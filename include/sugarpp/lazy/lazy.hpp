/*****************************************************************//**
 * \file   lazy.hpp
 * \brief  Kotlin's lazy implementation
 *
 * \author Peter
 * \date   July 2022
 *********************************************************************/

#pragma once

#include <mutex>
#include <optional>
#include <atomic>
#ifdef SugarPPNamespace
namespace SugarPP
{
#endif
	enum class ThreadSafetyMode
	{
		Synchronized,
		Publication,
		None
	};

	template<typename Initializer, typename T = decltype(std::declval<Initializer>()())>
	class Lazy
	{
	private:
		std::optional<T> m_value;
		Initializer m_initializer;
		ThreadSafetyMode m_mode;
		std::mutex m_lock;
		std::atomic_bool m_hasValue{ false };
	public:
		Lazy(Initializer&& initializer, ThreadSafetyMode threadSafetyMode = ThreadSafetyMode::Synchronized) :
			m_initializer(std::move(initializer)),
			m_mode{ threadSafetyMode }
		{
		}

		T& value()
		{
			switch (m_mode)
			{
				case ThreadSafetyMode::Publication:
				{
					if (m_hasValue.load(std::memory_order_acquire))
						return m_value.value();

					auto result = m_initializer();
					{
						std::lock_guard guard{ m_lock };
						if (!m_hasValue)
							m_value.emplace(std::move(result));
					}
					m_hasValue.store(true, std::memory_order_release);
					return m_value.value();
				}
				case ThreadSafetyMode::Synchronized:
				{
					if (m_hasValue)
						return m_value.value();

					{
						std::lock_guard guard{ m_lock };
						if (m_hasValue)					// one thread will get false here, so continue initialization
							return m_value.value();		// other threads will have to check whether it's initialized or not again after acquisition of the lock
						m_value.emplace(m_initializer());
					}
					m_hasValue.store(true, std::memory_order_release);
					return m_value.value();
				}
				default:
					m_value.emplace(m_initializer());
					m_hasValue.store(true, std::memory_order_relaxed);
					return m_value.value();
			}
		}

		bool isInitialized() const
		{
			return m_hasValue;
		}
	};
	//template<typename Initializer>
	//Lazy(Initializer)->Lazy<decltype(Initializer{}()), Initializer>;

	//template<typename Initializer>
	//Lazy(Initializer, ThreadSafetyMode)->Lazy<decltype(Initializer{}()), Initializer>;

#ifdef SugarPPNamespace
}
#endif