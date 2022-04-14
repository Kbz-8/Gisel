#ifndef __SINGLETON__
#define __SINGLETON__


template <typename T>
class Singleton
{
	public:
		Singleton(const Singleton&) = delete;
		Singleton(Singleton&&) noexcept = default;
		Singleton &operator=(const Singleton&) = delete;
		Singleton &operator=(Singleton&&) noexcept = default;

		inline static T& get()
		{
			static T instance;
			return instance;
		}

	protected:
		Singleton() = default;
		virtual ~Singleton() = default;
};

#endif // __SINGLETON__
