// ********* se agregaron los comentarios por el puerto serial de manera correcta *********

//********** requerimientos de memoria flash ********************

#include <Preferences.h>

Preferences preferences;

// ********* requerimientos de teclado matricial *********

// importa libreria Keypad
#include <Keypad.h>
// define numero de filas
const uint8_t ROWS = 2;
// define numero de columnas
const uint8_t COLS = 3;
// define la distribucion de teclas
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'}};
// pines correspondientes a las filas
uint8_t colPins[COLS] = {2, 4, 15}; // 5 = C, 4 = D, 15 = E
// pines correspondientes a las columnas
uint8_t rowPins[ROWS] = {18, 19}; // 19 = A, 18 = B
// crea objeto con los prametros creados previamente
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ********* requerimientos de tira de neopixeles *********

#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define Pin 23
#define CantidadLed 6

// Driver
// WS2812 - NEO_KHZ800
// WS2811 - NEO_KHZ400
// LED
// GRB - NEO_GRB
// RGB - NEO_RGB
// RGBW - NEO_RGBW

Adafruit_NeoPixel tira = Adafruit_NeoPixel(CantidadLed, Pin, NEO_GRB + NEO_KHZ800);

// ******* declaracion de variables globales y funciones del teclado con los neopixeles ********

// *************** Prototipos de funciones ************
void loopPrincipal();
void menu();
void seleccionSubmenu();
void subMenuEventos();
void submenuManual();
void confiEventos();
void sumaTiempo();
void restaTiempo();
void editarTiempoMayor();
void editarTiempoMenor();
void resetearTiempo();
void seleccionNeopixel();
void controlIndicador();
void seleccionGuardadotiempo();
void pulsoRelay();

//********* Variables globales ***********

// variables para neopixeles

unsigned int /*tiempos[10] = {1000, 2000, 3000, 4000, 5000},*/ cambioTiempos[10] = {1, 1, 1, 1, 1};
unsigned int tiempos[10];
int incrementos[10] = {0, 0, 0, 0, 0}, rojo[3] = {255, 0, 0};

// variables globales
boolean bandera1 = false, bandera2 = false, indicadorNeopixel = false, bandera4 = false, bandera5 = false, banderaMensaje1 = true, banderaMensaje2 = true, banderaMensaje3 = true, banderaMensaje4 = true, banderaMensaje5 = true;

// char neoSeleccionado = '0';

int neoSeleccionado = 0;

unsigned int tiemposConfig[5] = {1, 10, 100, 1000, 10000}, tiempoNeoSeleccionado = 0, increDecre = 0;

unsigned int tiempoUltimaPulsacion = 0, ultimoTiempo = 0;

// variables para neopixel de opciones (banderas)

int banderaNeo1 = 0;

// variables de relay

int pinRelay = 13;

void setup()
{
    Serial.begin(9600); // iniciar puerto serial

    // ********* iniciacion de tira de neopixeles *******

#if defined(__AVR_ATtiny85__)
    if (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
#endif

    tira.begin();
    tira.setBrightness(100);
    tira.show();

    // obtencion de datos desde la memoria flash

    preferences.begin("my-app", false);

    tiempos[0] = preferences.getUInt("tiempos0", 1000);
    tiempos[1] = preferences.getUInt("tiempos1", 2000);
    tiempos[2] = preferences.getUInt("tiempos2", 3000);
    tiempos[3] = preferences.getUInt("tiempos3", 4000);
    tiempos[4] = preferences.getUInt("tiempos4", 5000);

    // inicializar en pin del relay como salida

    pinMode(pinRelay, OUTPUT);
    
    digitalWrite(pinRelay, HIGH);
}

//********* CODIGO PRINCIPAL *************

void loop()
{
    loopPrincipal();
}

//********* FUNCIONES *************

void loopPrincipal()
{
    // programa principal neo indicador apagado

    if (banderaMensaje1 == true)
    {
        Serial.println("\n*******************************************************************************************");
        Serial.println("\nPresione del 1 - 5 para encender el neopixel correspondiente o 6 entrar al menu"); // envia a monitor serial la tecla presionada
        banderaMensaje1 = false;
    }

    char key = keypad.getKey(); // comprueba que se haya presionado una tecla

    if (key)
    {

        if (key != '6')
        {
            Serial.println("\n*******************************************************************************************");
            Serial.print("\nHa pulsado el boton "); // envia a monitor serial la tecla presionada
            Serial.println(key);                    // envia a monitor serial la tecla presionada
        }
        else
        {
            Serial.println("\n*******************************************************************************************");
            Serial.print("\nHa elegido entrar al menu"); // envia a monitor serial la tecla presionada
        }

        switch (key)
        {
        case '1':
            controlNeopixel(255, 0, 0, 0, tiempos[0], '1'); // codificacion para dato 1, color rojo, tiempo 1 seg, dato
            break;

        case '2':
            controlNeopixel(0, 255, 0, 1, tiempos[1], '2'); // codificacion para dato 2, color rojo, tiempo 2 seg, dato
            break;

        case '3':
            controlNeopixel(0, 0, 255, 2, tiempos[2], '3'); // codificacion para dato 3, color rojo, tiempo 3 seg, dato
            break;

        case '4':
            controlNeopixel(255, 255, 0, 3, tiempos[3], '4'); // codificacion para dato 4, color rojo, tiempo 4 seg, dato
            break;

        case '5':
            controlNeopixel(255, 0, 255, 4, tiempos[4], '5'); // codificacion para dato 5, color rojo, tiempo 5 seg, dato
            break;

        case '6':
            banderaNeo1 = 1;
            controlIndicador();
            Serial.println("\n*******************************************************************************************");
            Serial.println("\nEntro al menu de configuraciones"); // envia a monitor serial la tecla presionada
            menu();
            break;
        }
    }
    else
    {
        ;
    }
}

void menu()
{
    // programa menu para seleccionar el neopixel que se quiere modificar indicador rojo

    bandera1 = true;

    while (bandera1 == true)
    {

        if (banderaMensaje3 == true)
        {
            Serial.println("\n*******************************************************************************************");
            Serial.println("\nPresione del 1 - 5 para modificar el neopixel correspondiente o 6 salir del menu"); // envia a monitor serial la tecla presionada
            banderaMensaje3 = false;
        }

        char key = keypad.getKey(); // comprueba que se haya presionado una tecla

        if (key)
        {

            if (key != '6')
            {
                Serial.println("\n*******************************************************************************************");
                Serial.print("\nHa elegido modificar el neopixel: "); // envia a monitor serial la tecla presionada
                Serial.println(key);                                  // envia a monitor serial la tecla presionada
                banderaNeo1 = 2;
                controlIndicador();
            }
            else
            {
                Serial.println("\n*******************************************************************************************");
                Serial.print("\nHa elegido salir del menu"); // envia a monitor serial la tecla presionada
                banderaNeo1 = 0;
                controlIndicador();
            }

            switch (key)
            {
            case '1':
                seleccionSubmenu('1'); // envia el neopixel que se quiere configurar
                break;

            case '2':
                seleccionSubmenu('2'); // envia el neopixel que se quiere configurar
                break;

            case '3':
                seleccionSubmenu('3'); // envia el neopixel que se quiere configurar
                break;

            case '4':
                seleccionSubmenu('4'); // envia el neopixel que se quiere configurar
                break;

            case '5':
                seleccionSubmenu('5'); // envia el neopixel que se quiere configurar
                break;

            case '6':
                Serial.println("\n*******************************************************************************************");
                Serial.println("\nSalio del menu"); // envia a monitor serial la tecla presionada
                banderaMensaje1 = true;
                banderaMensaje3 = true;
                bandera1 = false;
                break;
            }
        }
        else
        {
            ;
        }
    }
}

void seleccionSubmenu(char key)
{
    // programa submenu para seleccionar el el modo de modificacion del neo seleccionado, neo indicador verde

    neoSeleccionado = key - '0';
    neoSeleccionado = neoSeleccionado - 1;
    bandera4 = true;

    while (bandera4 == true)
    {
        if (banderaMensaje4 == true)
        {
            Serial.println("\n*******************************************************************************************");
            Serial.print("\nQue metodo desea ocupar para modificar el timer del neopixel: "); // envia a monitor serial la tecla presionada
            Serial.println(neoSeleccionado + 1);
            Serial.print("\npresione 1 para eventos, 2 para manual, 6 para salir:"); // envia a monitor serial la tecla presionada

            banderaMensaje4 = false;
        }

        char key = keypad.getKey(); // comprueba que se haya presionado una tecla

        if (key)
        {
            switch (key)
            {
            case '1':
                banderaNeo1 = 3;
                controlIndicador();
                subMenuEventos();
                banderaMensaje4 = true;
                break;
            case '2':
                banderaNeo1 = 5;
                controlIndicador();
                subMenuManual();
                banderaMensaje4 = true;
                break;
            case '6':
                banderaNeo1 = 1;
                controlIndicador();
                bandera4 = false;
                banderaMensaje4 = true;
                banderaMensaje3 = true;
                break;
            }
        }
    }
}

void subMenuEventos()
{
    // programa configuracion por eventos apagado, neo indicador azul

    Serial.println("\n*******************************************************************************************");
    Serial.print("\nsubmenuEventos de configuracion de neopixel: "); // envia a monitor serial la tecla presionada
    Serial.println(neoSeleccionado + 1);
    bandera5 = true;

    while (bandera5 == true)
    {

        seleccionNeopixel(neoSeleccionado);

        if (banderaMensaje2 == true)
        {
            Serial.println("\n*******************************************************************************************");
            Serial.print("\nTimer de neopixel: "); // envia a monitor serial la tecla presionada
            Serial.println(tiempoNeoSeleccionado);

            Serial.print("\nPresione 1 para comenzar el timer o 6 para salir"); // envia a monitor serial la tecla presionada

            banderaMensaje2 = false;
        }

        char key = keypad.getKey(); // comprueba que se haya presionado una tecla

        if (key)
        {
            switch (key)
            {
            case '1':
                banderaNeo1 = 4;
                controlIndicador();
                confiEventos(neoSeleccionado);
                banderaMensaje2 = true;
                break;
            case '6':
                banderaNeo1 = 2;
                controlIndicador();
                bandera5 = false;
                banderaMensaje3 = true;
                banderaMensaje2 = true;
                break;
            }
        }
    }
}

void confiEventos(int neoSeleccionado)
{
    // programa configuracion por eventos encendido, neo indicador 255, 255, 0

    int R, G, B;
    uint32_t ColorApagado = tira.Color(0, 0, 0);
    boolean banderaEvento = true;
    unsigned long tiempoConfi = 0;

    switch (neoSeleccionado)
    {
    case 0:
        R = 255;
        G = 0;
        B = 0;
        break;
    case 1:
        R = 0;
        G = 255;
        B = 0;
        break;
    case 2:
        R = 0;
        G = 0;
        B = 255;
        break;
    case 3:
        R = 255;
        G = 255;
        B = 0;
        break;
    case 4:
        R = 255;
        G = 0;
        B = 255;
        break;
    }

    uint32_t ColorActual = tira.Color(R, G, B);

    pulsoRelay();

    tira.setPixelColor(neoSeleccionado, ColorActual);
    tira.show();

    tiempoUltimaPulsacion = millis();

    while (banderaEvento == true)
    {
        if (banderaMensaje5 == true)
        {
            Serial.print("\nPresione 1 para terminar el timer"); // envia a monitor serial la tecla presionada
            banderaMensaje5 = false;
        }

        char key = keypad.getKey(); // comprueba que se haya presionado una tecla

        if (key == '1')
        {
            banderaNeo1 = 3;
            controlIndicador();

            tiempoConfi = millis() - tiempoUltimaPulsacion;
            tiempos[neoSeleccionado] = tiempoConfi;
            banderaEvento = false;

            pulsoRelay();

            tira.setPixelColor(neoSeleccionado, ColorApagado);
            tira.show();
            banderaMensaje5 = true;

            Serial.printf("Current counter value: %u\n", tiempos[neoSeleccionado]);

            seleccionGuardadotiempo(neoSeleccionado);
        }
    }
}

void subMenuManual()
{
    // programa configuracion manual, neo indicador 0, 255, 255

    bandera2 = true;

    Serial.println("\n*******************************************************************************************");
    Serial.print("\nsubmenuManual de configuracion de neopixel: "); // envia a monitor serial la tecla presionada
    Serial.println(neoSeleccionado);

    while (bandera2 == true)
    {
        seleccionNeopixel(neoSeleccionado);

        if (banderaMensaje2 == true)
        {
            Serial.println("\n*******************************************************************************************");
            Serial.println("\n1:suma incremento, 2:resta incremento, 3: incremento, 4: decremento, 5: tiempo en 0 o 6 aceptar la configuracion"); // envia a monitor serial la tecla presionada
            Serial.print("\nTimer de neopixel: ");                                                                                                // envia a monitor serial la tecla presionada
            Serial.println(tiempoNeoSeleccionado);
            Serial.print("\nIncrementos o decrementos: "); // envia a monitor serial la tecla presionada
            Serial.println(increDecre);

            banderaMensaje2 = false;
        }

        char key = keypad.getKey(); // comprueba que se haya presionado una tecla

        if (key)
        {
            Serial.print("\nHa elegido la opcion: "); // envia a monitor serial la tecla presionada
            Serial.println(key);                      // envia a monitor serial la tecla presionada

            switch (key)
            {
            case '1':
                sumaTiempo(); // envia el neopixel que se quiere configurar
                break;

            case '2':
                restaTiempo(); // envia el neopixel que se quiere configurar
                break;

            case '3':
                editarTiempoMayor(); // envia el neopixel que se quiere configurar
                break;

            case '4':
                editarTiempoMenor(); // envia el neopixel que se quiere configurar
                break;

            case '5':
                resetearTiempo(); // envia el neopixel que se quiere configurar
                break;

            case '6':
                banderaNeo1 = 2;
                controlIndicador();
                Serial.println("\nSalir del submenuManual"); // envia a monitor serial la tecla presionada
                bandera2 = false;
                banderaMensaje3 = true;
                banderaMensaje2 = true;
                break;
            }
            seleccionGuardadotiempo(neoSeleccionado);
        }
        else
        {
            ;
        }
    }
}

void sumaTiempo()
{
    Serial.println(" sumar tiempo"); // envia a monitor serial la tecla presionada

    tiempos[neoSeleccionado] += cambioTiempos[neoSeleccionado];

    banderaMensaje2 = true;
}

void restaTiempo()
{
    Serial.println(" resta tiempo"); // envia a monitor serial la tecla presionada
    banderaMensaje2 = true;

    if (cambioTiempos[neoSeleccionado] <= tiempos[neoSeleccionado])
    {
        tiempos[neoSeleccionado] -= cambioTiempos[neoSeleccionado];
    }
    else
    {
        // comando no valido
    }
}

void editarTiempoMayor()
{
    Serial.println(" editar a tiempo mayor"); // envia a monitor serial la tecla presionada
    banderaMensaje2 = true;

    if (incrementos[neoSeleccionado] == 4)
    {
        incrementos[neoSeleccionado] == 4;
    }
    else
    {
        incrementos[neoSeleccionado]++;
    }

    cambioTiempos[neoSeleccionado] = tiemposConfig[incrementos[neoSeleccionado]];
}

void editarTiempoMenor()
{
    Serial.println(" editar a tiempo menor"); // envia a monitor serial la tecla presionada
    banderaMensaje2 = true;

    if (incrementos[neoSeleccionado] == 0)
    {
        incrementos[neoSeleccionado] == 0;
    }
    else
    {
        incrementos[neoSeleccionado]--;
    }
    cambioTiempos[neoSeleccionado] = tiemposConfig[incrementos[neoSeleccionado]];
}

void resetearTiempo()
{
    Serial.println(" Resetear"); // envia a monitor serial la tecla presionada
    banderaMensaje2 = true;

    tiempos[neoSeleccionado] = 0;
}

void seleccionNeopixel(int neoSeleccionado)
{
    tiempoNeoSeleccionado = tiempos[neoSeleccionado];
    increDecre = cambioTiempos[neoSeleccionado];
}

void controlNeopixel(int R, int G, int B, int numNeo, unsigned long t, char dato)
{ // control de prendido

    unsigned long tiempoEnteros = 0, tiempoDecimales = 0;

    Serial.print("\nneopixel seleccionado: "); // envia a monitor serial la tecla presionada
    Serial.println(numNeo);
    Serial.print("\ntiempo encendido: "); // envia a monitor serial la tecla presionada
    tiempoEnteros = t / 1000;
    Serial.print(tiempoEnteros);
    Serial.print(".");
    tiempoDecimales = t - (tiempoEnteros * 1000);
    Serial.print(tiempoDecimales);
    Serial.println(" segundos.");

    ultimoTiempo = t;

    indicadorNeopixel = true;

    pulsoRelay();

    tiempoUltimaPulsacion = millis();

    uint32_t ColorActual = tira.Color(R, G, B);
    uint32_t ColorApagado = tira.Color(0, 0, 0);

    tira.setPixelColor(numNeo, ColorActual);
    tira.show();

    while (indicadorNeopixel == true)
    {
        char key = keypad.getKey(); // comprueba que se haya presionado una tecla

        if (millis() >= tiempoUltimaPulsacion + ultimoTiempo)
        {
            pulsoRelay();
            tira.setPixelColor(numNeo, ColorApagado);
            tira.show();

            Serial.println("\nSe apago por tiempo"); // envia a monitor serial la tecla presionada

            indicadorNeopixel = false;
        }
        else
        {
            if (key == dato)
            {
                pulsoRelay();
                tira.setPixelColor(numNeo, ColorApagado);
                tira.show();

                Serial.println("\nSe apago por doble pulsado"); // envia a monitor serial la tecla presionada

                indicadorNeopixel = false;
            }
        }
    }
    banderaMensaje1 = true;
}

void controlIndicador()
{
    uint32_t ColorMenu = tira.Color(255, 0, 0);
    uint32_t ColorEleccionNeo = tira.Color(0, 255, 0);
    uint32_t ColorEventos0 = tira.Color(0, 0, 255);
    uint32_t ColorEventos1 = tira.Color(255, 255, 0);
    uint32_t ColorManual = tira.Color(0, 255, 255);
    uint32_t ColorApagado = tira.Color(0, 0, 0);
    int neoIndicador = 5;

    switch (banderaNeo1)
    {
    case 0:
        tira.setPixelColor(neoIndicador, ColorApagado);
        tira.show();
        break;
    case 1:
        // eleccion del neo
        tira.setPixelColor(neoIndicador, ColorMenu);
        tira.show();
        break;
    case 2:
        // metodo modificador
        tira.setPixelColor(neoIndicador, ColorEleccionNeo);
        tira.show();
        break;
    case 3:
        // eventos apagado
        tira.setPixelColor(neoIndicador, ColorEventos0);
        tira.show();
        break;
    case 4:
        // eventos prendido
        tira.setPixelColor(neoIndicador, ColorEventos1);
        tira.show();
        break;
    case 5:
        // modificador manual
        tira.setPixelColor(neoIndicador, ColorManual);
        tira.show();
        break;

    default:
        break;
    }
}

void seleccionGuardadotiempo(int neoSeleccionado)
{
    switch (neoSeleccionado)
    {
    case 0:
        preferences.putUInt("tiempos0", tiempos[neoSeleccionado]);
        break;
    case 1:
        preferences.putUInt("tiempos1", tiempos[neoSeleccionado]);
        break;
    case 2:
        preferences.putUInt("tiempos2", tiempos[neoSeleccionado]);
        break;
    case 3:
        preferences.putUInt("tiempos3", tiempos[neoSeleccionado]);
        break;
    case 4:
        preferences.putUInt("tiempos4", tiempos[neoSeleccionado]);
        break;
    }
}

void pulsoRelay()
{
    digitalWrite(pinRelay, LOW);
    delay(500);
    digitalWrite(pinRelay, HIGH);
}