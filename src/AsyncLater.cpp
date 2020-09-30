

#include <Rcpp.h>
#include <mutex>
#include <later_api.h>
#include "AsyncLater.h"

namespace httpgd
{
    namespace asynclater
    {

        std::mutex later_mutex;

        struct AsyncLaterData
        {
            void (*func)(void *);
            void *data;
        } rsdat;

        void later(void (*func)(void *), void *data, double secs)
        {
            later_mutex.lock();
            rsdat.data = data;
            rsdat.func = func;
            later::later([](void *data) {
                try
                {
                    Rcpp::unwindProtect([](void *data) { 
                        auto d = static_cast<AsyncLaterData *>(data);
                        d->func(d->data);
                        return R_NilValue;
                    }, data);
                }
                catch (...)
                {
                } // make sure mutex gets unlocked (does not work)
                later_mutex.unlock();
            },
                         &rsdat, secs);
        }

        void awaitLater()
        {
            later_mutex.lock();
            later_mutex.unlock();
        }

    } // namespace asynclater
} // namespace httpgd
