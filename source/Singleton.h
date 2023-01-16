#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton
{
public:
    virtual ~Singleton()
    {}

    static T &getInstance()
    {
        if (singleton == nullptr)
            singleton = new T();

        return *singleton;
    }

    static T &getBuilt()
    {
        return *singleton;
    }

    void destroy()
    {
        delete singleton;
    }

protected:
    Singleton()
    {}

    static T *singleton;
};

template<typename T>
T *Singleton<T>::singleton = nullptr;

#endif // SRWSO_SINGLETON_H