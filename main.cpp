#include "metodos.hpp"
#include "parametros.hpp"
#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <csignal>
#include <cstdlib>
#include <string>
#include <vector>

volatile bool running = true;

/// @brief Maneja señal de interrupción (Ctrl+C)
/// @param signal Número de señal
void signal_handler(int signal) {
    std::cout << "Recibida señal de interrupción. Cerrando..." << std::endl;
    running = false;
}

/// @brief Muestra ayuda de línea de comandos
void mostrar_ayuda() {
    std::cout << "Uso: ./simulador.o [OPCIONES]\n"
              << "Opciones:\n"
              << "  -N NUM          Número de partículas (default: 2000)\n"
              << "  -L NUM          Lado del cubo (default: 2000.0)\n"
              << "  -dt NUM         Paso de tiempo (default: 0.05)\n"
              << "  -v NUM    Velocidad máxima (default: 300.0)\n"
              << "  -r NUM    Radio máximo (default: 5.0)\n"
              << "  -m NUM   Masa máxima (default: 5.0)\n"
              << "  -h              Mostrar esta ayuda\n"
              << "Ejemplo: ./simulador.o -N 1000 -L 1500.0 -dt 0.01\n";
}

/// @brief Función principal
/// @param argc Cantidad de argumentos
/// @param argv Array de argumentos
/// @return 0 si éxito, 1 si error
int main(int argc, char* argv[]) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    ParametrosSimulacion params;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-N" && i + 1 < argc) {
            params.N = std::atoi(argv[++i]);
        } else if (arg == "-L" && i + 1 < argc) {
            params.L = std::atof(argv[++i]);
        } else if (arg == "-dt" && i + 1 < argc) {
            params.dt = std::atof(argv[++i]);
        } else if (arg == "-v" && i + 1 < argc) {
            params.vel_max = std::atof(argv[++i]);
        } else if (arg == "-r" && i + 1 < argc) {
            params.rad_max = std::atof(argv[++i]);
        } else if (arg == "-m" && i + 1 < argc) {
            params.masa_max = std::atof(argv[++i]);
        } else if (arg == "-h") {
            mostrar_ayuda();
            return 0;
        } else {
            std::cerr << "Argumento desconocido: " << arg << std::endl;
            mostrar_ayuda();
            return 1;
        }
    }

    std::vector<Particula> particulas;
    generar_particulas(particulas, params);

    zmq::context_t ctx(1);
    zmq::socket_t socket(ctx, zmq::socket_type::pub);
    
    try {
        socket.bind("tcp://*:5555");
        std::cout << "Simulación iniciada con parámetros:\n"
                  << "  Partículas: " << params.N << "\n"
                  << "  Cubo L: " << params.L << "\n"
                  << "  dt: " << params.dt << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } catch (const zmq::error_t& e) {
        std::cerr << "Error al bindear el puerto: " << e.what() << std::endl;
        return 1;
    }

    std::vector<double> buffer_particulas;
    const int publish_ms = 16;

    int step = 0;
    while (running) {
        actualizar_simulacion(particulas);
        empaquetar_datos(particulas, buffer_particulas);

        try {
            std::vector<double> buffer_extendido;
            buffer_extendido.reserve(1 + buffer_particulas.size());
            
            buffer_extendido.push_back(params.L);
            buffer_extendido.insert(buffer_extendido.end(), 
                                  buffer_particulas.begin(), 
                                  buffer_particulas.end());
            
            zmq::message_t msg(buffer_extendido.size() * sizeof(double));
            std::memcpy(msg.data(), buffer_extendido.data(), 
                       buffer_extendido.size() * sizeof(double));
            socket.send(msg, zmq::send_flags::none);
            
            if (step % 100 == 0 && particulas.size() > 0) {
                std::cout << "Step " << step << ": " 
                          << "Partículas: " << (buffer_particulas.size() / 4) 
                          << ", L=" << params.L
                          << std::endl;
            }
        } catch (const zmq::error_t& e) {
            std::cerr << "Error enviando datos: " << e.what() << std::endl;
            break;
        }

        step++;
        std::this_thread::sleep_for(std::chrono::milliseconds(publish_ms));
    }

    std::cout << "Simulación finalizada. Total steps: " << step << std::endl;
    return 0;
}
