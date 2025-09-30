#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <iomanip>

class Hryukoiner {
private:
    std::mutex output_mutex;
    std::atomic<bool> mining_complete{false};
    
    // Константы для майнинга
    static constexpr long long ITERATIONS_PER_BATCH = 100000;
    static constexpr double COINS_PER_BATCH = 0.01;
    static constexpr double TARGET_COINS = 1.0;
    static constexpr double MILLIONAIRE_TARGET = 1000000.0;
    
public:
    double mineWithThreads(int num_threads) {
        std::atomic<long long> total_iterations{0};
        std::vector<std::thread> threads;
        mining_complete = false;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Запускаем потоки
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([this, &total_iterations, &mining_complete]() {
                while (!mining_complete.load()) {
                    // Выполняем итерации майнинга
                    for (int j = 0; j < ITERATIONS_PER_BATCH && !mining_complete.load(); ++j) {
                        // Имитация работы майнинга
                        volatile int dummy = 0;
                        for (int k = 0; k < 100; ++k) {
                            dummy += k * k;
                        }
                    }
                    
                    total_iterations += ITERATIONS_PER_BATCH;
                    
                    // Проверяем, достигли ли цели
                    double current_coins = static_cast<double>(total_iterations) / ITERATIONS_PER_BATCH * COINS_PER_BATCH;
                    if (current_coins >= TARGET_COINS && !mining_complete.exchange(true)) {
                        break;
                    }
                }
            });
        }
        
        // Ждем завершения майнинга
        while (!mining_complete.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        
        // Останавливаем все потоки
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        std::chrono::duration<double> duration = end_time - start_time;
        return duration.count();
    }
    
    void calculateAndPrintResults(int num_threads, double mining_time) {
        std::lock_guard<std::mutex> lock(output_mutex);
        
        double iterations_per_second = (ITERATIONS_PER_BATCH / COINS_PER_BATCH * TARGET_COINS) / mining_time * num_threads;
        double time_for_million = (MILLIONAIRE_TARGET / TARGET_COINS) * mining_time;
        
        // Конвертируем в дни/годы
        double seconds_in_day = 86400.0;
        double seconds_in_year = 31536000.0; // 365 дней
        
        double days = time_for_million / seconds_in_day;
        double years = time_for_million / seconds_in_year;
        
        std::cout << "=== Результаты для " << num_threads << " ПК(потоков) ===" << std::endl;
        std::cout << "Время майнинга 1 Хрюкоина: " << std::fixed << std::setprecision(2) 
                  << mining_time << " секунд" << std::endl;
        std::cout << "Скорость майнинга: " << std::scientific << std::setprecision(2) 
                  << iterations_per_second << " итераций/сек" << std::endl;
        std::cout << std::fixed;
        
        std::cout << "Время для становления миллионером:" << std::endl;
        if (days < 365) {
            std::cout << "  " << std::setprecision(1) << days << " дней" << std::endl;
        } else {
            std::cout << "  " << std::setprecision(1) << years << " лет" << std::endl;
        }
        
        if (years > 100) {
            std::cout << "  💀 Василию стоит рассмотреть другие варианты заработка!" << std::endl;
        } else if (years > 10) {
            std::cout << "  ⚠️  Майнинг Хрюкоинов - это долгосрочная инвестиция!" << std::endl;
        } else {
            std::cout << "  ✅ Реальная цель! Удачи Василию!" << std::endl;
        }
        std::cout << std::endl;
    }
    
    void runAnalysis() {
        std::vector<int> thread_configs = {1, 2, 4};
        std::vector<double> mining_times;
        
        std::cout << "🚀 Запуск анализа майнинга Хрюкоинов..." << std::endl;
        std::cout << "Хрюкоинист-1: 100000 итераций = 0.01 Хрюкоина" << std::endl;
        std::cout << "Цель: 1 Хрюкоин" << std::endl << std::endl;
        
        // Измеряем время для каждой конфигурации
        for (int num_threads : thread_configs) {
            double mining_time = mineWithThreads(num_threads);
            mining_times.push_back(mining_time);
            calculateAndPrintResults(num_threads, mining_time);
        }
        
        // Анализ эффективности
        std::cout << "📊 Анализ эффективности:" << std::endl;
        for (size_t i = 1; i < mining_times.size(); ++i) {
            double speedup = mining_times[0] / mining_times[i];
            double efficiency = speedup / thread_configs[i] * 100;
            std::cout << thread_configs[i] << " потока(ов) vs 1 поток: " 
                      << std::fixed << std::setprecision(2) << speedup << "x ускорение, "
                      << std::setprecision(1) << efficiency << "% эффективности" << std::endl;
        }
    }
};

int main() {
    Hryukoiner miner;
    miner.runAnalysis();
    return 0;
}
