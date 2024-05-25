#!/usr/local/bin/cpx
#|

    void
test_void()
{
    int dums[]{1,2,3,4}; // immetate manipulated resource references
    INFO(FUNC_MSG("lambda without returning a value"));
    RunAndFinally {
        [&]{ // Run
            INFO(VARVALS(dums));
            RunAndFinally {
                [&]{ // nested Run
                    INFO("nested Run");
                },
                [&]{ // nested Finally
                    INFO("nested Finally");
                }
            }
            ;
        },
        [&]{ // Finally
            ranges::for_each(dums,[](int& d_){ d_=0; });
            INFO(VARVALS(dums));
        }
    }
    ;
}

    int
test_int()
{
    int dums[]{1,2,3,4}; // immetate manipulated resource references
    INFO(FUNC_MSG("lambda returning a value"));
    return RunAndFinally {
        [&]{ // Run
            INFO(VARVALS(dums));
            return
                RunAndFinally {
                    [&]{ // nested Run
                        INFO("nested Run");
                        return 11;
                    },
                    [&]{ // nested Finally
                        INFO("nested Finally");
                    }
                }
                .return_value();
        },
        [&]{ // Finally
            ranges::for_each(dums,[](int& d_){ d_=0; });
            INFO(VARVALS(dums));
        }
    }
    .return_value();
}


#(
    test_void();
    return test_int();
#)

