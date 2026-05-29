#include "Lista.h"
#include <utility>

Lista::Lista(): _primer(NULL) , _ultimo(NULL)
{

}

Lista::~Lista()
{
	Limpiar();
}

void Lista::Limpiar()
{
	Elemento* actual = _primer;
	while (actual != NULL)
	{
		Elemento* siguiente = actual->GetSiguiente();
		delete actual;
		actual = siguiente;
	}
	_primer = NULL;
	_ultimo = NULL;
}

void Lista::OrdenarPorNombre(bool ascendente)
{
	if (_primer == NULL || _primer->GetSiguiente() == NULL)
	{
		return;
	}

	for (Elemento* i = _primer; i != NULL; i = i->GetSiguiente())
	{
		for (Elemento* j = i->GetSiguiente(); j != NULL; j = j->GetSiguiente())
		{
			bool intercambiar = ascendente ? (i->GetNombre() > j->GetNombre())
				: (i->GetNombre() < j->GetNombre());
			if (intercambiar)
			{
				std::string nombreTemp = i->GetNombre();
				int cantidadTemp = i->GetCantidad();
				i->SetNombre(j->GetNombre());
				i->SetCantidad(j->GetCantidad());
				j->SetNombre(nombreTemp);
				j->SetCantidad(cantidadTemp);
			}
		}
	}
}

void Lista::OrdenarPorCantidad(bool ascendente)
{
	if (_primer == NULL || _primer->GetSiguiente() == NULL)
	{
		return;
	}

	for (Elemento* i = _primer; i != NULL; i = i->GetSiguiente())
	{
		for (Elemento* j = i->GetSiguiente(); j != NULL; j = j->GetSiguiente())
		{
			bool intercambiar = ascendente ? (i->GetCantidad() > j->GetCantidad())
				: (i->GetCantidad() < j->GetCantidad());
			if (!intercambiar && i->GetCantidad() == j->GetCantidad())
			{
				intercambiar = ascendente ? (i->GetNombre() > j->GetNombre())
					: (i->GetNombre() < j->GetNombre());
			}
			if (intercambiar)
			{
				std::string nombreTemp = i->GetNombre();
				int cantidadTemp = i->GetCantidad();
				i->SetNombre(j->GetNombre());
				i->SetCantidad(j->GetCantidad());
				j->SetNombre(nombreTemp);
				j->SetCantidad(cantidadTemp);
			}
		}
	}
}

Elemento* Lista::BuscarPorNombre(const std::string& nombre)
{
	Elemento* actual = _primer;
	while (actual != NULL)
	{
		if (actual->GetNombre() == nombre)
		{
			return actual;
		}
		actual = actual->GetSiguiente();
	}
	return NULL;
}

Elemento* Lista::ObtenerEnIndice(int indice)
{
	if (indice < 0)
	{
		return NULL;
	}
	Elemento* actual = _primer;
	int i = 0;
	while (actual != NULL && i < indice)
	{
		actual = actual->GetSiguiente();
		i++;
	}
	if (i == indice)
	{
		return actual;
	}
	return NULL;
}

void Lista::Agregar(Elemento* elemento)
{
	/*IMPLEMENTE ESTE METODO
		Dado un objeto de tipo puntero a Elemento agréguelo a la lista.

		Tenga pendiente que el atributo _primer guarda un puntero al primer elemento de la lista y que _ultimo es un puntero al último elemento de la lista
	*/
	if (!elemento)
	{
		return;
	}
	if (_primer == NULL)
	{
		_primer = elemento;
		_ultimo = elemento;
		elemento->SetSiguiente(NULL);
		return;
	}
	_ultimo->SetSiguiente(elemento);
	_ultimo = elemento;
	_ultimo->SetSiguiente(NULL);
}

bool Lista::Eliminar(int indice)
{
	/*
	IMPLEMENTE ESTE METODO
	Dado un indice entero elimine el objeto en esa posición.  Ejemplo si el parámetro indice es igual a 0 se eliminará el primer elemento.

	Esta función retorna un booleano,  retorno verdadero si el índice está contenido en la lista.

	Ejemplo, si la lista tiene 4 elementos y se pide eliminar el índice 9 este método retornará falso porque ese índice no está contenido en la lista.
	*/
	if (indice < 0 || _primer == NULL)
	{
		return false;
	}

	Elemento* actual = _primer;
	Elemento* anterior = NULL;
	int i = 0;

	while (actual != NULL && i < indice)
	{
		anterior = actual;
		actual = actual->GetSiguiente();
		i++;
	}

	if (actual == NULL)
	{
		return false;
	}

	if (anterior == NULL)
	{
		_primer = actual->GetSiguiente();
	}
	else
	{
		anterior->SetSiguiente(actual->GetSiguiente());
	}

	if (actual == _ultimo)
	{
		_ultimo = anterior;
	}

	delete actual;
	return true;
}
