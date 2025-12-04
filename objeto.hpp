#ifndef OBJETO_HPP
#define OBJETO_HPP

#include <array>
#include <vector>

struct ParametrosSimulacion;

class Particula {
private:
    const ParametrosSimulacion& params;

public:
    std::array<double,3> pos; 
    std::array<double,3> vel; 
    double masa;              
    double radio;             
    
    Particula(const std::array<double,3>& p,
              const std::array<double,3>& v,
              double m, double r,
              const ParametrosSimulacion& pms);

    ~Particula();

    void mover();
    
    void rebotar_paredes();
    
    void rebotar_particula(Particula& otra);
};

#endif
