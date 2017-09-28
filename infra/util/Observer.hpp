#ifndef INFRA_OBSERVER_HPP
#define INFRA_OBSERVER_HPP

#include "infra/util/IntrusiveList.hpp"
#include <cassert>

namespace infra
{
    template<class ObserverType_, class Helper = typename ObserverType_::SingleHelper>
    class Subject;

    template<class Descendant, class SubjectType_>
    class Observer
        : public IntrusiveList<Observer<Descendant, SubjectType_>>::NodeType
    {
    public:
        using SubjectType = SubjectType_;
        using SingleHelper = void;

    protected:
        Observer() = default;
        Observer(SubjectType& subject);                                                                                 //TICS !INT#001

        Observer(const Observer& other) = delete;
        Observer& operator=(const Observer& other) = delete;

        ~Observer();

        SubjectType& Subject() const;
        bool Attached() const;

        void Attach(SubjectType& subject);
        void Detach();

    private:
        SubjectType* subject = nullptr;
    };

    template<class Descendant, class SubjectType_>
    class SingleObserver
    {
    public:
        using SubjectType = SubjectType_;
        using SingleHelper = SubjectType_;

    protected:
        SingleObserver() = default;
        SingleObserver(SubjectType& subject);                                                                           //TICS !INT#001

        SingleObserver(const SingleObserver& other) = delete;
        SingleObserver& operator=(const SingleObserver& other) = delete;

        ~SingleObserver();

        SubjectType& Subject() const;
        bool Attached() const;

        void Attach(SubjectType& subject);
        void Detach();

    private:
        SubjectType* subject = nullptr;
    };

    template<class ObserverType_>
    class Subject<ObserverType_, void>
    {
    private:
        using ObserverType = ObserverType_;

    protected:
        Subject() = default;
        Subject(const Subject&) = delete;
        Subject& operator=(const Subject&) = delete;
        ~Subject();

    public:
        template<class F>
            void NotifyObservers(F callback) const;
        template<class F>
            void NotifyObservers(F callback);

    private:
        using SubjectType = typename ObserverType::SubjectType;
        friend class Observer<ObserverType_, SubjectType>;

        void RegisterObserver(Observer<ObserverType_, SubjectType>* observer);
        void UnregisterObserver(Observer<ObserverType_, SubjectType>* observer);

    private:
        IntrusiveList<Observer<ObserverType_, SubjectType>> observers;
    };

    template<class ObserverType_, class Helper>
    class Subject
    {
    private:
        using ObserverType = ObserverType_;
        using SubjectType = typename ObserverType::SubjectType;

    protected:
        Subject() = default;
        Subject(const Subject&) = delete;
        Subject& operator=(const Subject&) = delete;
        ~Subject();

    public:
        bool HasObserver() const;
        ObserverType& GetObserver() const;

    private:
        friend class SingleObserver<ObserverType_, SubjectType>;
        void RegisterObserver(SingleObserver<ObserverType_, SubjectType>* observer);
        void UnregisterObserver(SingleObserver<ObserverType_, SubjectType>* observer);

    private:
        SingleObserver<ObserverType_, SubjectType>* observer = nullptr;
    };

    ////    Implementation    ////

    template<class Descendant, class SubjectType_>
    Observer<Descendant, SubjectType_>::Observer(SubjectType& subject)
    {
        Attach(subject);
    }

    template<class Descendant, class SubjectType_>
    Observer<Descendant, SubjectType_>::~Observer()
    {
        Detach();
    }

    template<class Descendant, class SubjectType_>
    typename Observer<Descendant, SubjectType_>::SubjectType& Observer<Descendant, SubjectType_>::Subject() const
    {
        return *subject;
    }

    template<class Descendant, class SubjectType_>
    bool Observer<Descendant, SubjectType_>::Attached() const
    {
        return subject != nullptr;
    }

    template<class Descendant, class SubjectType_>
    void Observer<Descendant, SubjectType_>::Attach(SubjectType& subject)
    {
        Detach();
        this->subject = &subject;
        static_cast<infra::Subject<Descendant>&>(subject).RegisterObserver(this);
    }

    template<class Descendant, class SubjectType_>
    void Observer<Descendant, SubjectType_>::Detach()
    {
        if (subject)
            static_cast<infra::Subject<Descendant>*>(subject)->UnregisterObserver(this);
        subject = nullptr;
    }

    template<class Descendant, class SubjectType_>
    SingleObserver<Descendant, SubjectType_>::SingleObserver(SubjectType& subject)
    {
        Attach(subject);
    }

    template<class Descendant, class SubjectType_>
    SingleObserver<Descendant, SubjectType_>::~SingleObserver()
    {
        Detach();
    }

    template<class Descendant, class SubjectType_>
    typename SingleObserver<Descendant, SubjectType_>::SubjectType& SingleObserver<Descendant, SubjectType_>::Subject() const
    {
        assert(subject != nullptr);
        return *subject;
    }

    template<class Descendant, class SubjectType_>
    bool SingleObserver<Descendant, SubjectType_>::Attached() const
    {
        return subject != nullptr;
    }

    template<class Descendant, class SubjectType_>
    void SingleObserver<Descendant, SubjectType_>::Attach(SubjectType& subject)
    {
        Detach();
        this->subject = &subject;
        static_cast<infra::Subject<Descendant>*>(this->subject)->RegisterObserver(this);
    }

    template<class Descendant, class SubjectType_>
    void SingleObserver<Descendant, SubjectType_>::Detach()
    {
        if (Attached())
            static_cast<infra::Subject<Descendant>*>(this->subject)->UnregisterObserver(this);
        subject = nullptr;
    }

    template<class ObserverType>
    Subject<ObserverType, void>::~Subject()
    {
        assert(observers.empty());
    }

    template<class ObserverType>
    template<class F>
    void Subject<ObserverType, void>::NotifyObservers(F callback) const
    {
        for (typename IntrusiveList<Observer<ObserverType, SubjectType>>::const_iterator i = observers.begin(); i != observers.end(); )
        {
            Observer<ObserverType, SubjectType>& observer = *i;
            ++i;
            callback(static_cast<const ObserverType&>(observer));
        }
    }

    template<class ObserverType>
    template<class F>
    void Subject<ObserverType, void>::NotifyObservers(F callback)
    {
        for (typename IntrusiveList<Observer<ObserverType, SubjectType>>::iterator i = observers.begin(); i != observers.end();)
        {
            Observer<ObserverType, SubjectType>& observer = *i;
            ++i;
            callback(static_cast<ObserverType&>(observer));
        }
    }

    template<class ObserverType>
    void Subject<ObserverType, void>::RegisterObserver(Observer<ObserverType, SubjectType>* observer)
    {
        observers.push_back(static_cast<ObserverType&>(*observer));
    }

    template<class ObserverType>
    void Subject<ObserverType, void>::UnregisterObserver(Observer<ObserverType, SubjectType>* observer)
    {
        observers.erase(static_cast<ObserverType&>(*observer));
    }

    template<class ObserverType, class Helper>
    Subject<ObserverType, Helper>::~Subject()
    {
        assert(observer == nullptr);
    }

    template<class ObserverType, class Helper>
    bool Subject<ObserverType, Helper>::HasObserver() const
    {
        return observer != nullptr;
    }

    template<class ObserverType, class Helper>
    ObserverType& Subject<ObserverType, Helper>::GetObserver() const
    {
        assert(observer != nullptr);
        return static_cast<ObserverType&>(*observer);
    }

    template<class ObserverType, class Helper>
    void Subject<ObserverType, Helper>::RegisterObserver(SingleObserver<ObserverType, SubjectType>* observer)
    {
        assert(this->observer == nullptr);
        this->observer = observer;
    }

    template<class ObserverType, class Helper>
    void Subject<ObserverType, Helper>::UnregisterObserver(SingleObserver<ObserverType, SubjectType>* observer)
    {
        (void)observer; // Avoid warning in release mode                                                                //TICS !CFL#024
        assert(this->observer == observer);
        this->observer = nullptr;
    }
}

#endif
