/**
 * @file metodos.hpp
 * @brief Métodos de generación y actualización de partículas
 * @ingroup Methods
 */

#include "metodos.hpp"
#include <random>
#include <cmath>
#include <iostream>
#include <omp.h>

void generar_particulas(std::vector<Particula>& particulas, const ParametrosSimulacion& params) {
    std::random_device rd;
    std::mt19937 rng(rd());

    std::uniform_real_distribution<double> dist_rad(1.0, params.rad_max);
    std::uniform_real_distribution<double> dist_pos(dist_rad(rng), params.L - dist_rad(rng));
    std::uniform_real_distribution<double> dist_vel(-params.vel_max, params.vel_max);
    std::uniform_real_distribution<double> dist_mas(1.0, params.masa_max);

    particulas.clear();
    particulas.reserve(params.N);

    while (particulas.size() < static_cast<size_t>(params.N)) {
        std::array<double,3> p = { dist_pos(rng), dist_pos(rng), dist_pos(rng) };
        bool ok = true;

        for (const auto& q : particulas) {
            double dx = p[0] - q.pos[0];
            double dy = p[1] - q.pos[1];
            double dz = p[2] - q.pos[2];
            double d = std::sqrt(dx*dx + dy*dy + dz*dz);
            if (d < 2.0 * params.rad_max) { ok = false; break; }
        }

        if (ok) {
            std::array<double,3> v = { dist_vel(rng), dist_vel(rng), dist_vel(rng) };
            Particula part(p, v, dist_mas(rng), dist_rad(rng), params);
            particulas.push_back(part);
        }
    }
}

void actualizar_simulacion(std::vector<Particula>& particulas) {
    const size_t N = particulas.size();
    
    #pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
        particulas[i].mover();
    }
    
    #pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
        particulas[i].rebotar_paredes();
    }
    
    #pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = i + 1; j < N; ++j) {
            particulas[i].rebotar_particula(particulas[j]);
        }
    }
}

void empaquetar_datos(const std::vector<Particula>& particulas, std::vector<double>& buffer) {
    buffer.clear();
    buffer.resize(particulas.size() * 4);
    
    #pragma omp parallel for
    for (size_t i = 0; i < particulas.size(); ++i) {
        const auto& p = particulas[i];
        size_t idx = i * 4;
        buffer[idx + 0] = p.pos[0];
        buffer[idx + 1] = p.pos[1];
        buffer[idx + 2] = p.pos[2];
        buffer[idx + 3] = p.radio;
    }
}
