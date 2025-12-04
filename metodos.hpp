#ifndef METODOS_HPP
#define METODOS_HPP

#include "objeto.hpp"
#include "parametros.hpp"
#include <vector>

void generar_particulas(std::vector<Particula>& particulas, const ParametrosSimulacion& params);

void actualizar_simulacion(std::vector<Particula>& particulas);

void empaquetar_datos(const std::vector<Particula>& particulas, std::vector<double>& buffer);

#endif
