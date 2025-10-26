// queue.hpp

//#ifndef __cpp_lib_hardware_interference_size
//    constexpr std::size_t const
//hardware_constructive_interference_size(2*sizeof(std::max_align_t));
//#else it is defined by the C++ runtime librarary
//#endif

    template <typename T>
    class
Queue
{
  public:
        using
    element_type = T
    ;
        using
    atomic_type = std::atomic_size_t
    ;
  protected:
        template <typename V>
        struct
    Element // holds a singe V item
    {
            using
        atomic_type = Queue::atomic_type
        ;
            typename std::aligned_storage_t<sizeof(V), alignof(V)>
        m_storage // unassigned bytes, alligned in which a single V fits
        ;
            alignas(hardware_constructive_interference_size)
            atomic_type mutable
        m_idx_=0
        ;
      public:
        ~Element() noexcept
        {
            if (m_idx_.load(std::memory_order::acquire)) {
                //destruct
                reinterpret_cast<V*>(&m_storage)->~V();
            }
        }
            template <typename... Args>
            constexpr void
        safe(const auto& _head, Args&&... _args_) noexcept
        {
            //construct
            new(&m_storage) V(std::forward<Args>(_args_)...);

            m_idx_.store( _head+1 , std::memory_order::release);
            m_idx_.notify_all();
        }
            void
        remove(const auto& _head)
        {
            //destruct
            reinterpret_cast<V*>(&m_storage)->~V();

            m_idx_.store( _head+1 , std::memory_order::release);
            m_idx_.notify_all();
        }
            constexpr V&&
        move() noexcept
        {
            return reinterpret_cast<V&&>(m_storage);
        }
    } // conatiner_t
    ;
  private:
        const std::size_t
    m_capacity
    ;
        std::allocator< Element<T> >
    m_allocator
    ;
        Element<T>*
    m_elements
    ;
        alignas(hardware_constructive_interference_size) atomic_type
    m_head
    ;
        alignas(hardware_constructive_interference_size) atomic_type
    m_tail
    ;
  protected:
        template <typename... Args>
        constexpr void
    emplace(Args&&... args) noexcept
    {
            const auto
        tail(m_head.fetch_add(1,std::memory_order::acquire))
        ;
            const auto
        head((tail/m_capacity)*2)
        ;
            auto*
        element_ptr(&m_elements[tail%m_capacity])
        ;
        for(;;) {
            const auto now(element_ptr->m_idx_.load(std::memory_order::acquire));
            if ( now== head ) {
                break;
            }
            element_ptr->m_idx_.wait(now, std::memory_order::relaxed);
        }
        element_ptr->safe( head ,std::forward<Args>(args)...);
    } // emplace()

  public:
       atomic_short
    channel_senders_reference_count     // shall only manipulated by channel::send<T> constructor/destructor
    ,
    channel_receivers_reference_count   // shall only manipulated by channel::send<T> constructor/destructor
    ;
        constexpr
    Queue(const Queue&)
        = delete
    ;
        constexpr
    Queue &operator=(const Queue&)
        = delete
    ;
        constexpr explicit
    Queue(const std::size_t capacity = UINT16_MAX)
    : m_capacity(capacity)
    , m_allocator(std::allocator<Element<T>>())
    , m_head(0ul)
    , m_tail(0ul)
    , channel_senders_reference_count(0)
    , channel_receivers_reference_count(0)
    {
        m_elements = m_allocator.allocate(m_capacity + 1);
        for (const auto& i : repeat(m_capacity) ) {
            new(&m_elements[i]) Element<T>();
        }
    }

    ~Queue(void) noexcept
    {
        for (const auto& i : repeat(m_capacity)) {
            m_elements[i].~Element();
        }
        m_allocator.deallocate(m_elements, m_capacity + 1);
    }
        constexpr void
    push( const T& v) noexcept
    {
        emplace(v);
    }
        constexpr void
    push( std::convertible_to<T> auto&& v) noexcept
    {
        emplace(std::forward<decltype(v)>(v));
    }
        constexpr T
    pop() noexcept
    {
            const auto
        tail(m_tail.fetch_add(1,std::memory_order::acquire))
        ;
            const auto
        head((tail/m_capacity)*2+1)
        ;
            auto*
        element_ptr(&m_elements[tail%m_capacity])
        ;
        for(;;) {
            const auto now(element_ptr->m_idx_.load(std::memory_order::acquire));
            if ( now== head ) {
                break;
            }
            element_ptr->m_idx_.wait(now,std::memory_order::relaxed);
        }
            std::optional<T>
        res = std::move(element_ptr->move())
        ;
        element_ptr->remove( head);
        return std::move(*res);
    } // pop()

        size_t
    size() const noexcept
    {
        return m_head.load(std::memory_order::acquire) - m_tail.load(std::memory_order::acquire);
    }
        size_t
    unused() const noexcept
    {
        return m_capacity- size();
    }
        bool
    empty() const noexcept
    {
        return ( !size() );
    }
        bool
    full() const noexcept
    {
        return ( !unused() );
    }
        void
    dump() const noexcept
    {
        INFO(VARVALS(m_head,m_tail,m_capacity,size(),empty()));
    }
} // Queue
;

________________________________________________________________________________________________________________________
class channel
{
// channel::recv and channel::send instances are always part of the the
// receiving and sending thread. The access therefor is single threaded.

  protected:
        template<class Elem_T>
        class
    base
    {
      protected:
            Queue<Elem_T>&
        m_queRef
        ;
            //explicit
        base(Queue<Elem_T>& _queRef_)
        : m_queRef(_queRef_)
        {
        }
            virtual
        ~base()
        {
        }
            Queue<Elem_T>&
        queRef()
            const noexcept
        {
            return m_queRef;
        }

      public:
            using
        data_type = Elem_T
        ;
            Queue<Elem_T>*
        operator -> ()
            const
            noexcept
        {
            return ( &(const_cast<decltype(this)>(this)->queRef()) );
        }
    } // base
    ;
  public:
        template<class Elem_T>
        class
    recv
    final // recv copies queue reference and should never do more, forbid adding members
    : public base<Elem_T>
    {
        using base<Elem_T>::queRef;
        using base<Elem_T>::operator ->;

      public:
            //explicit
        recv(Queue<Elem_T>& _queRef_)
            : base<Elem_T>(_queRef_)
        {
            queRef().channel_receivers_reference_count++;
        }
        ~recv()
        {
            //ASSERT( queRef().channel_receivers_reference_count > 0 );
            queRef().channel_receivers_reference_count--;
        }
            const recv&
            operator
        >> (Elem_T& e_) // Blocking read from Queue
            const noexcept
        {
            e_= operator->()->pop();
            return *this;
        }
            operator
        bool() // Return false when next >> would block.
            const noexcept
        {
            auto* q_ptr= this->operator->();
            if ( q_ptr->channel_senders_reference_count> 0 ) {
                // queue is filled or will filled again
                return true;
            }
            // otherwise, no new items will be pushed to the queue
            if ( q_ptr->size()> 0 ) {
                // still able to pop from queue
                return true;
            }
            // otherwise, queue is empty and not filled again
            return false;
        }
            bool operator
        !()
            const noexcept
        {
            return !operator bool();
        }
    } // class recv
    ;
        template<class Elem_T>
        class
    send final
    // send copies queue reference and should never do more, forbid adding members
    : public base<Elem_T>
    {
        using base<Elem_T>::queRef;
        using base<Elem_T>::operator-> ;

    public:
            //explicit
        send(Queue<Elem_T>& _queRef_)
            : base<Elem_T>(_queRef_)
        {
            queRef().channel_senders_reference_count++;
        }
        ~send()
        {
            //ASSERT( queRef().channel_senders_reference_count > 0 );
            queRef().channel_senders_reference_count--;
        }
            const send&
            operator
        << (Elem_T&& _e) // Blocking write from Queue
            const noexcept
        {
            operator->()->push( _e);
            return *this;
        }
            operator
        bool() // Return false when next << would block.
            const noexcept
        {
            auto* q= this->operator->();
            if ( q->channel_receivers_reference_count ) {
                // queue items are consumed or will be consumed again
                return true;
            }
            // otherwise, there are no receivers
            if ( q->unused()> 0 ) {
                // but we still can deliver to the queue
                return true;
            }
            // otherwise cannot deliver to queue
            return false;
        }
            bool operator
        !()
            const noexcept
        {
            return !operator bool();
        }
    } // class send
    ;
};
