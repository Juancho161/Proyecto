/**
 * @file objeto.hpp
 * @brief Clase Particula y métodos de colisión
 * @ingroup Core
 */

#include "objeto.hpp"
#include "parametros.hpp"
#include <cmath>
#include <omp.h>

Particula::Particula(const std::array<double,3>& p,
                     const std::array<double,3>& v,
                     double m, double r,
                     const ParametrosSimulacion& pms)
    : params(pms), pos(p), vel(v), masa(m), radio(r) {}
    
Particula::~Particula() {}

void Particula::mover() {
    pos[0] += vel[0] * params.dt;
    pos[1] += vel[1] * params.dt;
    pos[2] += vel[2] * params.dt;
}

void Particula::rebotar_paredes() {
    for (int i = 0; i < 3; ++i) {
        if (pos[i] < radio) {
            pos[i] = radio;
            vel[i] *= -1;
        } else if (pos[i] > params.L - radio) {
            pos[i] = params.L - radio;
            vel[i] *= -1;
        }
    }
}

void Particula::rebotar_particula(Particula& otra) {
    double rx = otra.pos[0] - pos[0];
    double ry = otra.pos[1] - pos[1];
    double rz = otra.pos[2] - pos[2];
    double d = std::sqrt(rx*rx + ry*ry + rz*rz);

    if (d <= 0.0) return;

    if (d < radio + otra.radio) {
        double n0 = rx / d;
        double n1 = ry / d;
        double n2 = rz / d;

        double dvx = vel[0] - otra.vel[0];
        double dvy = vel[1] - otra.vel[1];
        double dvz = vel[2] - otra.vel[2];

        double j = 2.0 * (dvx*n0 + dvy*n1 + dvz*n2) / (1.0/masa + 1.0/otra.masa);

        vel[0] -= j * n0 / masa;
        vel[1] -= j * n1 / masa;
        vel[2] -= j * n2 / masa;

        double overlap = (radio + otra.radio - d) * 0.5;

        pos[0] -= overlap * n0;
        pos[1] -= overlap * n1;
        pos[2] -= overlap * n2;
    }
}
