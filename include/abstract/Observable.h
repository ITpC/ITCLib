/* 
 * File:   Observable.h
 * Author: pk
 *
 * Created on 17 Июнь 2008 г., 1:41
 */

#ifndef _OBSERVABLE_H
#define	_OBSERVABLE_H

#include <abstract/Observer.h>
#include <boost/shared_ptr.hpp>

namespace itc
{
    namespace abstract
    {
        template <typename TModel> class IController
        {
        public:
            IController(){}
        protected:
            inline void notify(boost::shared_ptr<IView>& ref, const TModel& model_ref)
            {
                if(IView* tmpPtr=ref.get())
                {
                    tmpPtr->update(model_ref);
                }
            }
            virtual ~IController(){};
        };
    }
}


#endif	/* _OBSERVABLE_H */

