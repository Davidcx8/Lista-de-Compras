#include "Lista.h"
#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
Lista* lista = new Lista();
const string kDataFile = "lista_compras.txt";

struct Item
{
    string nombre;
    int cantidad;
};

vector<vector<Item>> historialDeshacer;
vector<vector<Item>> historialRehacer;

bool LeerEntero(const string& prompt, int& valor)
{
    cout << prompt;
    if (!(cin >> valor))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

void Pausa(const string& mensaje)
{
    cout << mensaje;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    cout << endl;
}

string Trim(const string& texto)
{
    size_t inicio = 0;
    while (inicio < texto.size() &&
           (texto[inicio] == ' ' || texto[inicio] == '\t' || texto[inicio] == '\r' || texto[inicio] == '\n'))
    {
        ++inicio;
    }
    size_t fin = texto.size();
    while (fin > inicio &&
           (texto[fin - 1] == ' ' || texto[fin - 1] == '\t' || texto[fin - 1] == '\r' || texto[fin - 1] == '\n'))
    {
        --fin;
    }
    return texto.substr(inicio, fin - inicio);
}

string LeerRuta(const string& prompt, const string& rutaPorDefecto)
{
    cout << prompt << " [" << rutaPorDefecto << "]: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string ruta;
    getline(cin, ruta);
    ruta = Trim(ruta);
    if (ruta.empty())
    {
        return rutaPorDefecto;
    }
    return ruta;
}

vector<Item> CapturarEstado()
{
    vector<Item> estado;
    Elemento* e = lista->GetPrimer();
    while (e != NULL)
    {
        Item item;
        item.nombre = e->GetNombre();
        item.cantidad = e->GetCantidad();
        estado.push_back(item);
        e = e->GetSiguiente();
    }
    return estado;
}

void RestaurarEstado(const vector<Item>& estado)
{
    lista->Limpiar();
    for (size_t i = 0; i < estado.size(); ++i)
    {
        lista->Agregar(new Elemento(estado[i].nombre, estado[i].cantidad));
    }
}

void GuardarEstadoParaDeshacer()
{
    historialDeshacer.push_back(CapturarEstado());
    historialRehacer.clear();
}

bool Deshacer()
{
    if (historialDeshacer.empty())
    {
        return false;
    }
    historialRehacer.push_back(CapturarEstado());
    RestaurarEstado(historialDeshacer.back());
    historialDeshacer.pop_back();
    return true;
}

bool Rehacer()
{
    if (historialRehacer.empty())
    {
        return false;
    }
    historialDeshacer.push_back(CapturarEstado());
    RestaurarEstado(historialRehacer.back());
    historialRehacer.pop_back();
    return true;
}

string EscaparCSV(const string& texto)
{
    string salida = "\"";
    for (size_t i = 0; i < texto.size(); ++i)
    {
        char c = texto[i];
        if (c == '"')
        {
            salida += "\"\"";
        }
        else
        {
            salida += c;
        }
    }
    salida += "\"";
    return salida;
}

bool ParsearCSVLinea(const string& linea, vector<string>& campos)
{
    campos.clear();
    string actual;
    bool enComillas = false;
    for (size_t i = 0; i < linea.size(); ++i)
    {
        char c = linea[i];
        if (enComillas)
        {
            if (c == '"')
            {
                if (i + 1 < linea.size() && linea[i + 1] == '"')
                {
                    actual += '"';
                    ++i;
                }
                else
                {
                    enComillas = false;
                }
            }
            else
            {
                actual += c;
            }
        }
        else
        {
            if (c == ',')
            {
                campos.push_back(actual);
                actual.clear();
            }
            else if (c == '"')
            {
                enComillas = true;
            }
            else
            {
                actual += c;
            }
        }
    }
    campos.push_back(actual);
    return !campos.empty();
}

string EscaparJSON(const string& texto)
{
    string salida;
    for (size_t i = 0; i < texto.size(); ++i)
    {
        char c = texto[i];
        switch (c)
        {
            case '\\': salida += "\\\\"; break;
            case '"': salida += "\\\""; break;
            case '\n': salida += "\\n"; break;
            case '\r': salida += "\\r"; break;
            case '\t': salida += "\\t"; break;
            default: salida += c; break;
        }
    }
    return salida;
}

bool ExtraerCampoCadena(const string& objeto, const string& campo, string& valor)
{
    string clave = "\"" + campo + "\"";
    size_t pos = objeto.find(clave);
    if (pos == string::npos)
    {
        return false;
    }
    pos = objeto.find(':', pos + clave.size());
    if (pos == string::npos)
    {
        return false;
    }
    pos = objeto.find('"', pos);
    if (pos == string::npos)
    {
        return false;
    }
    ++pos;
    string resultado;
    bool escape = false;
    for (; pos < objeto.size(); ++pos)
    {
        char c = objeto[pos];
        if (escape)
        {
            switch (c)
            {
                case 'n': resultado += '\n'; break;
                case 'r': resultado += '\r'; break;
                case 't': resultado += '\t'; break;
                case '\\': resultado += '\\'; break;
                case '"': resultado += '"'; break;
                default: resultado += c; break;
            }
            escape = false;
        }
        else if (c == '\\')
        {
            escape = true;
        }
        else if (c == '"')
        {
            valor = resultado;
            return true;
        }
        else
        {
            resultado += c;
        }
    }
    return false;
}

bool ExtraerCampoEntero(const string& objeto, const string& campo, int& valor)
{
    string clave = "\"" + campo + "\"";
    size_t pos = objeto.find(clave);
    if (pos == string::npos)
    {
        return false;
    }
    pos = objeto.find(':', pos + clave.size());
    if (pos == string::npos)
    {
        return false;
    }
    ++pos;
    while (pos < objeto.size() && (objeto[pos] == ' ' || objeto[pos] == '\t'))
    {
        ++pos;
    }
    size_t fin = pos;
    while (fin < objeto.size() && (objeto[fin] == '-' || (objeto[fin] >= '0' && objeto[fin] <= '9')))
    {
        ++fin;
    }
    if (fin == pos)
    {
        return false;
    }
    istringstream lectura(objeto.substr(pos, fin - pos));
    return static_cast<bool>(lectura >> valor);
}

bool GuardarEnArchivo(const string& ruta)
{
    ofstream archivo(ruta.c_str(), ios::trunc);
    if (!archivo.is_open())
    {
        return false;
    }

    Elemento* e = lista->GetPrimer();
    while (e != NULL)
    {
        archivo << e->GetNombre() << ';' << e->GetCantidad() << '\n';
        e = e->GetSiguiente();
    }

    return true;
}

bool CargarDesdeArchivo(const string& ruta)
{
    ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        return false;
    }

    string linea;
    while (getline(archivo, linea))
    {
        if (linea.empty())
        {
            continue;
        }

        size_t separador = linea.find_last_of(';');
        if (separador == string::npos)
        {
            continue;
        }

        string nombre = linea.substr(0, separador);
        string cantidadTexto = linea.substr(separador + 1);
        if (nombre.empty())
        {
            continue;
        }

        istringstream lectura(cantidadTexto);
        int cantidad = 0;
        if (!(lectura >> cantidad))
        {
            continue;
        }

        Elemento* existente = lista->BuscarPorNombre(nombre);
        if (existente != NULL)
        {
            existente->SetCantidad(existente->GetCantidad() + cantidad);
            continue;
        }

        lista->Agregar(new Elemento(nombre, cantidad));
    }

    return true;
}

bool ExportarCSV(const string& ruta)
{
    ofstream archivo(ruta.c_str(), ios::trunc);
    if (!archivo.is_open())
    {
        return false;
    }
    archivo << "nombre,cantidad\n";
    Elemento* e = lista->GetPrimer();
    while (e != NULL)
    {
        archivo << EscaparCSV(e->GetNombre()) << ',' << e->GetCantidad() << '\n';
        e = e->GetSiguiente();
    }
    return true;
}

bool ExportarJSON(const string& ruta)
{
    ofstream archivo(ruta.c_str(), ios::trunc);
    if (!archivo.is_open())
    {
        return false;
    }

    archivo << "[\n";
    Elemento* e = lista->GetPrimer();
    bool primero = true;
    while (e != NULL)
    {
        if (!primero)
        {
            archivo << ",\n";
        }
        archivo << "  {\"nombre\":\"" << EscaparJSON(e->GetNombre())
                << "\",\"cantidad\":" << e->GetCantidad() << "}";
        primero = false;
        e = e->GetSiguiente();
    }
    archivo << "\n]\n";
    return true;
}

bool ImportarCSV(const string& ruta, vector<Item>& items)
{
    items.clear();
    ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        return false;
    }

    string linea;
    bool primeraLinea = true;
    while (getline(archivo, linea))
    {
        if (!linea.empty() && linea[0] == '\xEF')
        {
            if (linea.size() >= 3 && linea[1] == '\xBB' && linea[2] == '\xBF')
            {
                linea = linea.substr(3);
            }
        }
        if (linea.empty())
        {
            continue;
        }
        if (primeraLinea)
        {
            primeraLinea = false;
            string lineaLower = linea;
            for (size_t i = 0; i < lineaLower.size(); ++i)
            {
                if (lineaLower[i] >= 'A' && lineaLower[i] <= 'Z')
                {
                    lineaLower[i] = static_cast<char>(lineaLower[i] - 'A' + 'a');
                }
            }
            if (lineaLower.find("nombre") != string::npos)
            {
                continue;
            }
        }

        vector<string> campos;
        if (!ParsearCSVLinea(linea, campos))
        {
            continue;
        }
        if (campos.size() < 2)
        {
            continue;
        }

        string nombre = Trim(campos[0]);
        if (nombre.empty())
        {
            continue;
        }

        string cantidadTexto = Trim(campos[1]);
        istringstream lectura(cantidadTexto);
        int cantidad = 0;
        if (!(lectura >> cantidad))
        {
            continue;
        }
        if (cantidad <= 0)
        {
            continue;
        }

        Item item;
        item.nombre = nombre;
        item.cantidad = cantidad;
        items.push_back(item);
    }

    return true;
}

bool ImportarJSON(const string& ruta, vector<Item>& items)
{
    items.clear();
    ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        return false;
    }

    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    if (contenido.size() >= 3 && contenido[0] == '\xEF' && contenido[1] == '\xBB' && contenido[2] == '\xBF')
    {
        contenido = contenido.substr(3);
    }
    size_t pos = 0;
    while (true)
    {
        size_t inicio = contenido.find('{', pos);
        if (inicio == string::npos)
        {
            break;
        }
        size_t fin = contenido.find('}', inicio);
        if (fin == string::npos)
        {
            break;
        }
        string bloque = contenido.substr(inicio + 1, fin - inicio - 1);

        string nombre;
        int cantidad = 0;
        if (ExtraerCampoCadena(bloque, "nombre", nombre) &&
            ExtraerCampoEntero(bloque, "cantidad", cantidad) &&
            !nombre.empty() && cantidad > 0)
        {
            Item item;
            item.nombre = nombre;
            item.cantidad = cantidad;
            items.push_back(item);
        }
        pos = fin + 1;
    }

    return true;
}

void AgregarItems(const vector<Item>& items)
{
    for (size_t i = 0; i < items.size(); ++i)
    {
        const Item& item = items[i];
        Elemento* existente = lista->BuscarPorNombre(item.nombre);
        if (existente != NULL)
        {
            existente->SetCantidad(existente->GetCantidad() + item.cantidad);
        }
        else
        {
            lista->Agregar(new Elemento(item.nombre, item.cantidad));
        }
    }
}


void Listar()
{
    cout << endl << endl;
    int i = 1;
    int totalArticulos = 0;
    int totalUnidades = 0;
    Elemento* e = lista->GetPrimer();
    while(e != NULL)
    {
        cout << i << "- " << e->GetNombre() << " " << e->GetCantidad() << endl;
        totalArticulos++;
        totalUnidades += e->GetCantidad();
        e = e->GetSiguiente();
        i++;
    }
    if(totalArticulos == 0)
    {
        cout << endl << "** Lista vacia **" << endl;
    }
    else
    {
        cout << endl << "Total articulos: " << totalArticulos << " | Total unidades: " << totalUnidades << endl;
    }
    cout << endl << endl;
}

void Introducir()
{
    string nombre;
    int cantidad;

    cout << "Introducir el nombre del articulo: ";
    cin >> ws;
    getline(cin, nombre);
    while (nombre.empty())
    {
        cout << "Nombre invalido. Intente de nuevo." << endl;
        cout << "Introducir el nombre del articulo: ";
        cin >> ws;
        getline(cin, nombre);
    }

    while (true)
    {
        if (!LeerEntero("Introduzca la cantidad del nuevo articulo: ", cantidad))
        {
            cout << "Entrada invalida. Intente de nuevo." << endl;
            continue;
        }
        if (cantidad <= 0)
        {
            cout << "Cantidad invalida. Use un numero positivo." << endl;
            continue;
        }
        break;
    }

    GuardarEstadoParaDeshacer();
    Elemento* existente = lista->BuscarPorNombre(nombre);
    if (existente != NULL)
    {
        existente->SetCantidad(existente->GetCantidad() + cantidad);
        GuardarEnArchivo(kDataFile);
        Pausa("-- Articulo actualizado. Presione ENTER para regresar al menu principal--");
        return;
    }

    Elemento* elemento = new Elemento(nombre, cantidad);
    lista->Agregar(elemento);
    GuardarEnArchivo(kDataFile);
    Pausa("-- Articulo agregado. Presione ENTER para regresar al menu principal--");
}

void Eliminar()
{
    int indice = 0;
    if (lista->GetPrimer() == NULL)
    {
        Listar();
        Pausa("Presione ENTER para continuar...");
        return;
    }
    Listar();
    while (!LeerEntero("Elija el articulo a eliminar => ", indice))
    {
        cout << "Entrada invalida. Intente de nuevo." << endl;
    }
    if (lista->ObtenerEnIndice(indice - 1) == NULL)
    {
        cout << "Indice invalido." << endl;
        return;
    }
    GuardarEstadoParaDeshacer();
    lista->Eliminar(indice - 1);
    GuardarEnArchivo(kDataFile);
    cout << endl;
    Listar();
    cout << endl;
}

void Editar()
{
    int indice = 0;
    if (lista->GetPrimer() == NULL)
    {
        Listar();
        Pausa("Presione ENTER para continuar...");
        return;
    }

    Listar();
    while (!LeerEntero("Elija el articulo a editar => ", indice))
    {
        cout << "Entrada invalida. Intente de nuevo." << endl;
    }

    Elemento* objetivo = lista->ObtenerEnIndice(indice - 1);
    if (objetivo == NULL)
    {
        cout << "Indice invalido." << endl;
        Pausa("Presione ENTER para continuar...");
        return;
    }

    GuardarEstadoParaDeshacer();
    cout << "Nuevo nombre (ENTER para mantener): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string nuevoNombre;
    getline(cin, nuevoNombre);
    if (!nuevoNombre.empty())
    {
        objetivo->SetNombre(nuevoNombre);
    }

    int nuevaCantidad = 0;
    while (true)
    {
        if (!LeerEntero("Nueva cantidad: ", nuevaCantidad))
        {
            cout << "Entrada invalida. Intente de nuevo." << endl;
            continue;
        }
        if (nuevaCantidad <= 0)
        {
            cout << "Cantidad invalida. Use un numero positivo." << endl;
            continue;
        }
        break;
    }
    objetivo->SetCantidad(nuevaCantidad);
    GuardarEnArchivo(kDataFile);
    Pausa("-- Articulo actualizado. Presione ENTER para continuar--");
}

void Ordenar()
{
    if (lista->GetPrimer() == NULL)
    {
        Listar();
        Pausa("Presione ENTER para continuar...");
        return;
    }

    int opcion = 0;
    cout << "1- Ordenar por nombre (A-Z)" << endl;
    cout << "2- Ordenar por nombre (Z-A)" << endl;
    cout << "3- Ordenar por cantidad (menor a mayor)" << endl;
    cout << "4- Ordenar por cantidad (mayor a menor)" << endl;
    if (!LeerEntero("Elija una opcion => ", opcion))
    {
        cout << "Entrada invalida. Intente de nuevo." << endl;
        return;
    }

    switch (opcion)
    {
        case 1:
            GuardarEstadoParaDeshacer();
            lista->OrdenarPorNombre(true);
        break;
        case 2:
            GuardarEstadoParaDeshacer();
            lista->OrdenarPorNombre(false);
        break;
        case 3:
            GuardarEstadoParaDeshacer();
            lista->OrdenarPorCantidad(true);
        break;
        case 4:
            GuardarEstadoParaDeshacer();
            lista->OrdenarPorCantidad(false);
        break;
        default:
            cout << "Opcion invalida" << endl;
            return;
    }

    GuardarEnArchivo(kDataFile);
    Pausa("-- Lista ordenada. Presione ENTER para continuar--");
}

void Exportar()
{
    int opcion = 0;
    cout << "1- Exportar CSV" << endl;
    cout << "2- Exportar JSON" << endl;
    if (!LeerEntero("Elija una opcion => ", opcion))
    {
        cout << "Entrada invalida." << endl;
        return;
    }

    bool ok = false;
    if (opcion == 1)
    {
        string ruta = LeerRuta("Ruta CSV de salida", "lista_compras.csv");
        ok = ExportarCSV(ruta);
    }
    else if (opcion == 2)
    {
        string ruta = LeerRuta("Ruta JSON de salida", "lista_compras.json");
        ok = ExportarJSON(ruta);
    }
    else
    {
        cout << "Opcion invalida" << endl;
        return;
    }

    if (ok)
    {
        Pausa("-- Exportacion completada. Presione ENTER para continuar--");
    }
    else
    {
        Pausa("-- Error al exportar. Presione ENTER para continuar--");
    }
}

void Importar()
{
    int opcion = 0;
    cout << "1- Importar CSV" << endl;
    cout << "2- Importar JSON" << endl;
    if (!LeerEntero("Elija una opcion => ", opcion))
    {
        cout << "Entrada invalida." << endl;
        return;
    }

    int modo = 0;
    cout << "1- Combinar con la lista actual" << endl;
    cout << "2- Reemplazar la lista actual" << endl;
    if (!LeerEntero("Elija una opcion => ", modo))
    {
        cout << "Entrada invalida." << endl;
        return;
    }
    if (modo != 1 && modo != 2)
    {
        cout << "Opcion invalida" << endl;
        return;
    }

    bool ok = false;
    vector<Item> items;
    if (opcion == 1)
    {
        string ruta = LeerRuta("Ruta CSV de entrada", "lista_compras.csv");
        ok = ImportarCSV(ruta, items);
    }
    else if (opcion == 2)
    {
        string ruta = LeerRuta("Ruta JSON de entrada", "lista_compras.json");
        ok = ImportarJSON(ruta, items);
    }
    else
    {
        cout << "Opcion invalida" << endl;
        return;
    }

    if (ok)
    {
        GuardarEstadoParaDeshacer();
        if (modo == 2)
        {
            lista->Limpiar();
        }
        AgregarItems(items);
        GuardarEnArchivo(kDataFile);
        Pausa("-- Importacion completada. Presione ENTER para continuar--");
    }
    else
    {
        Pausa("-- Error al importar. Presione ENTER para continuar--");
    }
}

void MostrarMenu()
{
    int opcion = 0;
    do
    {        
        cout << "1- Listar articulos" << endl;
        cout << "2- Introducir articulo" << endl;
        cout << "3- Eliminar articulo" << endl;
        cout << "4- Editar articulo" << endl;
        cout << "5- Ordenar lista" << endl;
        cout << "6- Deshacer" << endl;
        cout << "7- Rehacer" << endl;
        cout << "8- Exportar" << endl;
        cout << "9- Importar" << endl;
        cout << "10- Guardar y salir" << endl;
        if (!LeerEntero("Elija una opcion => ", opcion))
        {
            cout << "Entrada invalida. Intente de nuevo." << endl;
            continue;
        }
        
        switch(opcion)
        {
            case 1:
                Listar();
                Pausa("Presione ENTER para continuar...");
            break;
            case 2:
                Introducir();
            break;
            case 3:
                Eliminar();
            break;
            case 4:
                Editar();
            break;
            case 5:
                Ordenar();
            break;
            case 6:
                if (!Deshacer())
                {
                    Pausa("-- No hay acciones para deshacer. Presione ENTER--");
                }
                else
                {
                    GuardarEnArchivo(kDataFile);
                }
            break;
            case 7:
                if (!Rehacer())
                {
                    Pausa("-- No hay acciones para rehacer. Presione ENTER--");
                }
                else
                {
                    GuardarEnArchivo(kDataFile);
                }
            break;
            case 8:
                Exportar();
            break;
            case 9:
                Importar();
            break;
            case 10:
                return;
            break;
            default:
                cout << "Opcion invalida" << endl;
            break;
        }
    }while(opcion != 10);
}

int main()
{
    CargarDesdeArchivo(kDataFile);
    MostrarMenu();
    GuardarEnArchivo(kDataFile);
    delete lista;
    return 0;
}
