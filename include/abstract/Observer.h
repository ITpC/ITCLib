/* 
 * File:   Observer.h
 * Author: pk
 *
 * Created on 17 Июнь 2008 г., 1:42
 */

#ifndef _OBSERVER_H
#define	_OBSERVER_H

namespace itc
{
    namespace abstract
    {
        template <typename TModel> class IView
        {
        public:
            IView(){}
            
            inline void update(const TModel& ref)
            {
                onUpdate(ref);
            }
            
        protected:
            virtual void onUpdate(const TModel& ref)=0;
            virtual ~IView(){};
        };
    }
}

#endif	/* _OBSERVER_H */

