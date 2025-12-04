#ifndef OBJETO_HPP
#define OBJETO_HPP

#include <array>
#include <vector>

struct ParametrosSimulacion;

/// @brief Clase que representa una partícula
class Particula {
private:
    const ParametrosSimulacion& params;

public:
    std::array<double,3> pos; ///< Posición [x, y, z]
    std::array<double,3> vel; ///< Velocidad [vx, vy, vz]
    double masa;              ///< Masa
    double radio;             ///< Radio
    
    /// @brief Constructor
    /// @param p Posición inicial
    /// @param v Velocidad inicial
    /// @param m Masa
    /// @param r Radio
    /// @param pms Parámetros
    Particula(const std::array<double,3>& p,
              const std::array<double,3>& v,
              double m, double r,
              const ParametrosSimulacion& pms);

    ~Particula();

    /// @brief Actualiza posición según integración de Euler
    void mover();
    
    /// @brief Maneja colisiones con las paredes del cubo
    void rebotar_paredes();
    
    /// @brief Maneja colisión con otra partícula
    /// @param otra Otra partícula
    void rebotar_particula(Particula& otra);
};

#endif
