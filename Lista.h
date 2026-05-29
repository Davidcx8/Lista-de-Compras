#ifndef LISTA_H
#define LISTA_H

#include "Elemento.h"
#include <string>

class Lista
{
    public:
        Lista();
        ~Lista();
        void Agregar(Elemento* elemento);
        Elemento* GetPrimer() { return _primer; }
        void Limpiar();
        void OrdenarPorNombre(bool ascendente = true);
        void OrdenarPorCantidad(bool ascendente = true);
        Elemento* BuscarPorNombre(const std::string& nombre);
        Elemento* ObtenerEnIndice(int indice);
        bool Eliminar(int indice);

    private:
        Elemento* _primer;
        Elemento* _ultimo;
};
#endif
