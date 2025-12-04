#ifndef PARAMETROS_HPP
#define PARAMETROS_HPP

/// @brief Parámetros de la simulación
struct ParametrosSimulacion {
    double L = 2000.0;        ///< Lado del cubo
    double dt = 0.05;         ///< Paso de tiempo
    double vel_max = 300.0;   ///< Velocidad máxima
    double rad_max = 5.0;     ///< Radio máximo
    double masa_max = 5.0;    ///< Masa máxima
    int N = 2000;             ///< Número de partículas
};

#endif

