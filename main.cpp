#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>

/**
 * @brief Симуляция майнинга Хрюкоинов.
 *
 * Василий добывает 0.01 Хрюкоина за каждые 100000 итераций.
 * Нужно посчитать время на майнинг 1 Хрюкоина в 1, 2 и 4 потоках.
 */

std::mutex cout_mutex; ///< Мьютекс для синхронизации вывода

/**
 * @brief Функция для выполнения "майнинга" в одном потоке.
 * @param iterations Количество итераций, которое нужно выполнить
 */
void mine_iterations(size_t iterations) {
    volatile size_t dummy = 0; // фиктивная переменная для загрузки CPU
    for (size_t i = 0; i < iterations; ++i) {
        dummy += i % 7; // простая операция, чтобы компилятор не выкинул цикл
    }
}

/**
 * @brief Запускает многопоточный майнинг и возвращает время выполнения.
 * @param num_threads Количество потоков (1, 2 или 4)
 * @return Время выполнения в секундах
 */
double run_mining(int num_threads) {
    const size_t iterations_per_coin = 100'000'000; // 1 Хрюкоин = 10 млн итераций
    size_t per_thread = iterations_per_coin / num_threads;

    std::vector<std::thread> threads;
    auto start = std::chrono::high_resolution_clock::now();

    // запуск потоков
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(mine_iterations, per_thread);
    }

    // ожидание завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    return diff.count();
}

/**
 * @brief Точка входа в программу
 */
int main() {
    std::vector<int> thread_counts = { 1, 2, 4 };
    std::vector<double> results;

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "=== Mining simulation of Hryucoin ===\n\n";
    }

    for (int t : thread_counts) {
        double seconds = run_mining(t);
        results.push_back(seconds);
    }

    // вывод в виде таблицы
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << std::setw(10) << "Threads"
            << std::setw(20) << "Time (s)"
            << std::setw(20) << "Speedup" << "\n";
        std::cout << std::string(50, '-') << "\n";

        for (size_t i = 0; i < thread_counts.size(); ++i) {
            double speedup = results[0] / results[i];
            std::cout << std::setw(10) << thread_counts[i]
                << std::setw(20) << results[i]
                << std::setw(20) << speedup << "\n";
        }
    }

    // прогноз: миллионер
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "\n=== Forecast ===\n";
        for (size_t i = 0; i < thread_counts.size(); ++i) {
            double one_coin_time = results[i];
            double millionaire_seconds = one_coin_time * 1'000'000;
            double days = millionaire_seconds / (60 * 60 * 24);
            double years = days / 365.0;

            std::cout << "Threads: " << thread_counts[i] << "\n";
            std::cout << "- Seconds: " << millionaire_seconds << "\n";
            std::cout << "- Days:    " << days << "\n";
            std::cout << "- Years:   " << years << "\n\n";
        }
    }

    return 0;
}
