#include "tdas/list.h"
#include "tdas/extra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Estructura para representar un ticket con la información del ticket
typedef struct{
  char id[11] ;          // ID único del ticket (hasta 10 dígitos)
  char descripcion[201] ;// Descripción del problema (hasta 200 caracteres)
  char prioridad[6] ;    // Nivel de prioridad ("Alta", "Media", "Baja")
  char hora[20] ;        // Marca de tiempo de creación del ticket
}tipoTicket ;

/*
 * Función: mostrarMenuPrincipal
 * Descripción: Muestra el menú principal del sistema de gestión de tickets, 
 *              con opciones para registrar, modificar, mostrar, procesar y 
 *              buscar tickets.
 */
void mostrarMenuPrincipal() {
  limpiarPantalla();   // Limpia la pantalla de la consola
  puts("========================================");
  puts("     Sistema de Gestión de Tickets");
  puts("========================================");

  puts("1) Registrar ticket");
  puts("2) Asignar prioridad a ticket");
  puts("3) Mostrar lista de tickets pendientes");
  puts("4) Atender al siguiente paciente");
  puts("5) Buscar ticket por ID");
  puts("6) Salir");
}

/*
 * Función: not_in
 * Descripción: Verifica si un ID de ticket no existe en la lista de tickets.
 *              Retorna 1 si el ID no está, 0 si ya existe.
 */
int not_in(List *lista_tickets, char *id_buscada)
{
  // Inicializamos el recorrido en el primer ticket de la lista
  tipoTicket *ticket = list_first(lista_tickets) ;
  while(ticket != NULL) // Itera hasta finalizar la lista
  {
    // Compara el ID buscado con el ID del ticket actual
    if(strcmp(ticket->id, id_buscada) == 0) return 0; // Se encontró el ID
    // Avanza al siguiente ticket
    ticket = list_next(lista_tickets) ;
  }
  return 1 ; // Retorna 1 en caso de que no se haya encontrado el ID
}

/*
 * Función: priori_numeric
 * Descripción: Convierte una prioridad que está en texto ("Alta", "Media", "Baja") a un valor numérico 
 *              para facilitar las comparaciones (1 para Alta, 2 para Media, 3 para Baja).
 */
int priori_numeric(char *prioridad)
{
  // Asignación de valores numéricos según la prioridad
  if(strcmp(prioridad, "Alta") == 0) return 1 ;
  if(strcmp(prioridad, "Media") == 0) return 2 ;
  if(strcmp(prioridad, "Baja") == 0) return 3 ;
  return 0;// La prioridad es inválida
}

/*
 * Función: comp_Ticket_Prioridad_Hora
 * Descripción: Compara dos tickets según su prioridad y hora de creación para ordenarlos.
 *              Prioriza tickets con mayor prioridad (menor valor numérico) y, en caso de empate,
 *              por hora más temprana (prioriza los tickets más antiguos).
 */
int comp_Ticket_Prioridad_Hora(void *A, void *B)
{
  tipoTicket *ptrA = (tipoTicket *)A ;
  tipoTicket *ptrB = (tipoTicket *)B ;

  // Se asignan los valores numéricos con la función para comparar más fácilmente las prioridades
  int prioA = priori_numeric(ptrA->prioridad) ;
  int prioB = priori_numeric(ptrB->prioridad) ;

  // Comparación de prioridades
  if(prioA != prioB)
  {
    return prioA < prioB ? 1 : 0 ;
  }
  // Si las prioridades son iguales, compara las horas dejando las más antiguas primero
  return strcmp(ptrA->hora, ptrB->hora) < 0 ? 1 : 0 ;
}

/*
 * Función: obtener_hora
 * Descripción: Genera y retorna una cadena con la hora actual en formato 
 *              "HH:MM:SS". Retorna NULL si falla la asignación de memoria.
 */
char* obtener_hora()
{
  time_t tiempo ;
  struct tm *tm_info;
  // Se reserva memoria para la cadena "hora"
  char *hora = malloc(20);
  if(hora == NULL) return NULL; // Verifica el fallo de asignación de memoria
  time(&tiempo) ;
  tm_info = localtime(&tiempo);
  // Formatea sólo la hora en la cadena
  strftime(hora, 20, "%H:%M:%S", tm_info);

  return hora;
}

/*
 * Función: es_id_valido
 * Descripción: Valida un ID de ticket, asegurando que no esté vacío, tenga hasta 10 
 *              caracteres máximos.
 *              Retorna 1 si es válido, 0 si no lo es.
 */
int es_id_valido(const char *id)
{
  // Se verifica si el ID es NULL o está vacío
  if (id == NULL || id[0] == '\0') {
    return 0;
  }
  // Verifica longitud máxima (10)
  if (strlen(id) > 10) {
    return 0;
  }

  // Verifica que todos los caracteres del ID sean dígitos
  for (int i = 0; id[i] != '\0'; i++) {
    if (!isdigit(id[i])) {
      return 0;
    }
  }

  return 1; // Retorna 1 si el ID es válido
}

/*
 * Función: registrar_ticket
 * Descripción: Registra un nuevo ticket, solicitando ID y descripción al usuario, asignando 
 *              por defecto prioridad "Baja" y la hora actual. Lo inserta en la lista ordenada.
 */
void registrar_ticket(List *lista_tickets) {
  limpiarPantalla();     // Limpia la pantalla de la consola
  char auxiliar[11] ;    // Variable auxiliar para el ID ingresado

  // Solicita y valida el ID del ticket
  printf("Registrar nuevo paciente\n");
  printf("Por favor ingrese el número de su ticket: \n") ;
  
  // Iteración para pedir al usuario que ingrese un ID válido
  do{
    // Lee el ID (hasta 10 caracteres)
    scanf(" %10[^\n]", auxiliar) ;
    if(es_id_valido(auxiliar) == 0)
    {
      printf("El ID no puede estar vacío y sólo puede contener NÚMEROS con longitud máxima 10\n");
      printf("Por favor ingrese un número de ticket válido\n");
    }
  }while(es_id_valido(auxiliar)== 0); // Mientras el ID sea inválido seguirá pidiendo un ID

  // Verifica si el ID ya existe dentro de la lista
  if(!not_in(lista_tickets, auxiliar))
  {
    printf("Error: El ID ya existe.\n");
    presioneTeclaParaContinuar();
    return;
  }

  // Reserva memoria para el nuevo ticket
  tipoTicket *nuevo_ticket = (tipoTicket *)malloc(sizeof(tipoTicket));
  if(nuevo_ticket == NULL)
  {
    printf("ERROR: No se pudo asignar memoria para el ticket\n") ;
    presioneTeclaParaContinuar();
    return;
  }
  // Asigna el ID al ticket
  strcpy(nuevo_ticket->id, auxiliar);
  // Solicita la descripción del problema
  printf("Por favor ingrese una breve descripción de su problema (máx. 200 caracteres, mín. 1 caracter): \n");
  scanf(" %200[^\n]", nuevo_ticket->descripcion) ;
  // Asignación de prioridad por defecto
  strcpy(nuevo_ticket->prioridad, "Baja");

  // Obtiene y asigna la hora actual
  char *hora_actual = obtener_hora();
  if(hora_actual != NULL)
  {
    strcpy(nuevo_ticket->hora, hora_actual) ;
    // Libera la memoria de la hora
    free(hora_actual) ;
  }
  else{
    // Si la hora no se guardó efectivamente se guarda un mensaje que lo indique
    strcpy(nuevo_ticket->hora, "Hora no disponible") ;
  }
  limpiarPantalla();
  // Muestra los detalles del Ticket recién creado
  printf("Ticket creado como ID: %s\nDescripción: %s\nPrioridad: %s\nFecha y Hora: %s\n\n", 
    nuevo_ticket->id, nuevo_ticket->descripcion, nuevo_ticket->prioridad, nuevo_ticket->hora);
  // Inserta el ticket en la lista ordenada  
  list_sortedInsert(lista_tickets, nuevo_ticket, comp_Ticket_Prioridad_Hora);

  printf("¡Su ticket ha sido registrado con éxito!\n");
  presioneTeclaParaContinuar();
}

/*
 * Función: mostrar_Info_Ticket
 * Descripción: Muestra los detalles de un ticket específico, incluyendo ID, descripción, 
 *              prioridad y hora de creación.
 */
void mostrar_Info_Ticket(tipoTicket *ticket)
{
  // Muestra el ID del ticket
  printf("ID del Ticket: %s\n", ticket->id) ;
  // Muestra la descripción del problema
  printf("Descripción del problema: %s\n", ticket->descripcion) ;
  // Muestra la prioridad actual
  printf("Prioridad actual del Ticket: %s\n", ticket->prioridad) ;
  // Muestra la hora de creación
  printf("Hora de ingreso del Ticket: %s\n", ticket->hora) ;

  printf("\n") ;// Salto de línea
}

/*
 * Función: mostrar_lista_tickets
 * Descripción: Muestra todos los tickets pendientes en la lista, o un mensaje si está vacía.
 */
void mostrar_lista_tickets(List *lista_tickets) {
  limpiarPantalla();// Limpia la pantalla de la consola
  // Muestra el número de tickets en espera
  printf("Hay %d ticket(s) en espera\n\n", list_size(lista_tickets));
  // Verifica si la lista está vacía
  if(list_size(lista_tickets) == 0)
  {
    printf("No hay tickets en espera (lista vacía)\n");
    presioneTeclaParaContinuar();
    return;
  }

  // Muestra información de cada ticket
  printf("Información de tickets en espera: \n\n");
  // Se inicializa el recorrido en el primer elemento de la lista
  tipoTicket *ticket = list_first(lista_tickets) ;
  while(ticket != NULL)// Itera hasta que ticket sea NULL (último elemento)
  {
    mostrar_Info_Ticket(ticket);
    // Avanza al siguiente ticket
    ticket = list_next(lista_tickets) ;
  }

  presioneTeclaParaContinuar();
}

/*
 * Función: prioridad_valida
 * Descripción: Valida si una prioridad ingresada es "Alta", "Media" o "Baja" (sin distinguir
 *              mayúsculas/minúsculas, para mayor flexibilidad). Retorna 0 si es válida, 1 si no lo es.
 */
int prioridad_valida(char *ingresado)
{
  // Verifica si la prioridad es válida (ignorando mayúsculas)
  if(strcasecmp(ingresado, "Baja") == 0) return 0;
  if(strcasecmp(ingresado, "Media") == 0) return 0;
  if(strcasecmp(ingresado, "Alta") == 0) return 0;
  return 1;// Prioridad no válida
}

/*
 * Función: formateo_prioridad
 * Descripción: Formatea una cadena de prioridad para que la primera letra sea mayúscula
 *              y las demás minúsculas (ejemplo: "aLTa" → "Alta").
 */
void formateo_prioridad(char *prioridad)
{
  // Verifica que la cadena no esté vacía
  int i;
  if (prioridad[0] != '\0') {
    // Convierte a mayúscula la primera letra
    prioridad[0] = toupper(prioridad[0]);
    // Convierte el resto a minúsculas
    for (i = 1; prioridad[i] != '\0'; i++) {
      prioridad[i] = tolower(prioridad[i]);
    }
  }
}

/*
 * Función: asignar_prioridad
 * Descripción: Permite cambiar la prioridad de un ticket existente, validando que el ID exista
 *              y que la nueva prioridad sea válida, reordenando la lista después del cambio.
 */
void asignar_prioridad(List *lista_tickets)
{
  limpiarPantalla(); // Limpia la pantalla de la consola
  char buscado[11] ; // Variable para el ticket ingresado

  // Solicita el ID del ticket
  printf("Ingrese el ticket al cual desea cambiar su prioridad\n");
  scanf(" %10[^\n]", buscado) ;

  // Valida que el ID sea válido
  if (!es_id_valido(buscado)) {
    printf("ID no válido. Debe contener solo números y tener hasta 10 caracteres.\n");
    presioneTeclaParaContinuar();
    return;
  }

  // Verifica si el ticket existe dentro de la lista
  tipoTicket *ticket_Actual = list_first(lista_tickets);
  while (ticket_Actual != NULL) {
    if (strcmp(ticket_Actual->id, buscado) == 0) {
      // Muestra la prioridad actual del ticket
      printf("Prioridad Actual: %s\n", ticket_Actual->prioridad);

      // Se crea una variable temporal para confirmar que la prioridad sea válida
      char prioridad_temporal[11];

      // Solicita la nueva prioridad
      printf("Ingrese la prioridad que desea asignar\n");
      scanf(" %10[^\n]", prioridad_temporal) ;
      // Formatea la prioridad ingresada
      formateo_prioridad(prioridad_temporal);

      // Valida la nueva prioridad
      if (prioridad_valida(prioridad_temporal) != 0) {
        printf("Prioridad no válida\n");
        presioneTeclaParaContinuar();
        return;
      }
      // En caso que sea válida copia la prioridad al ticket
      strcpy(ticket_Actual->prioridad, prioridad_temporal);

      // Borra el ticket actual y lo vuelve a insertar de manera ordenada
      list_popCurrent(lista_tickets);
      list_sortedInsert(lista_tickets, ticket_Actual, comp_Ticket_Prioridad_Hora);
      printf("Prioridad cambiada con éxito\n") ;
      presioneTeclaParaContinuar();
      return;
    }
    // Avanza al siguiente ticket
    ticket_Actual = list_next(lista_tickets);
  }
  // Muestra error si el ticket no se encuentra
  printf("ERROR: El ticket no se encuentra en el sistema\n");
  presioneTeclaParaContinuar();
}

/*
 * Función: procesarTickets
 * Descripción: Procesa el ticket de mayor prioridad, mostrándolo y eliminándolo de la lista.
 *              Si no hay tickets en la lista, muestra un mensaje de error.
 */
void procesarTickets(List *lista_tickets)
{
  limpiarPantalla(); // Limpia la pantalla de la consola
  printf("Inicio del procesamiento de ticket\n") ;
  
  // Toma el primer ticket
  tipoTicket *ticket_Actual = list_first(lista_tickets);
  // Evalúa si el ticket tomado es NULL para mostrar un mensaje por pantalla
  if(ticket_Actual == NULL)
  {
    printf("No hay tickets para procesar\n");
    presioneTeclaParaContinuar();
    return;
  }
  // Ya que la lista está ordenada por prioridades y hora,
  // solo debemos tomar el primer ticket, no es necesario iterar
  // Borra el primer ticket
  tipoTicket *ticket_Procesado = list_popCurrent(lista_tickets);

  // Muestra los detalles del ticket procesado
  mostrar_Info_Ticket(ticket_Procesado);
  printf("El ticket ha sido procesado con éxito y ha sido eliminado de la lista\n");

  // Libera la memoria del ticket procesado
  free(ticket_Procesado);
  presioneTeclaParaContinuar();
}

/*
 * Función: buscar_Ticket
 * Descripción: Busca un ticket por su ID y muestra sus detalles si lo encuentra.
 *              Si no existe dentro de la lista, muestra un mensaje de error.
 */
void buscar_Ticket(List *lista_tickets)
{
  limpiarPantalla();  // Limpia la pantalla de la consola
  if (list_size(lista_tickets) == 0)
  {
    printf("La lista está vacía, por favor ingrese tickets para poder buscarlos\n");
    presioneTeclaParaContinuar();
    return;
  }
  printf("Inicio de la búsqueda\n") ;
  printf("Por favor ingrese el ID del ticket que desea buscar\n");
  char ticket_a_encontrar[11] ;
  // Lee el ID a buscar
  scanf(" %10[^\n]", ticket_a_encontrar) ;

  // Valida que el ID sea válido
  if (!es_id_valido(ticket_a_encontrar)) {
    printf("ID no válido. Debe contener solo números y tener hasta 10 caracteres.\n");
    presioneTeclaParaContinuar();
    return;
  }

  // Verifica si el ticket existe usando not_in
  if (not_in(lista_tickets, ticket_a_encontrar)) {
    printf("ERROR: El ticket no se encuentra en el sistema\n");
    presioneTeclaParaContinuar();
    return;
  }

  // Busca el ticket en la lista para mostrarlo
  tipoTicket *ticket_Actual = list_first(lista_tickets);
  while (ticket_Actual != NULL)
  {
    if (strcmp(ticket_Actual->id, ticket_a_encontrar) == 0)
    {
      // Muestra los detalles del ticket encontrado
      mostrar_Info_Ticket(ticket_Actual);
      presioneTeclaParaContinuar();
      return;
    }
    // Avanza al siguiente ticket
    ticket_Actual = list_next(lista_tickets);
  }
}

/*
 * Función: main
 * Descripción: Función principal que ejecuta el sistema de gestión de tickets, mostrando
 *              el menú y ejecutando las acciones según la opción que seleccione el usuario.
 */
int main() {
  // Variable para almacenar la opción seleccionada por el usuario
  char opcion;

  // Crea una lista para almacenar los tickets
  List *lista_tickets = list_create(); 

  // Bucle principal: muestra el menú y procesa opciones
  do {
    mostrarMenuPrincipal();

    // Solicita la opción del usuario
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion); // Nota el espacio antes de %c para consumir el
                           // newline anterior

    // Ejecuta la acción según la opción
    switch (opcion) {
    case '1':
      registrar_ticket(lista_tickets);  // Registra un nuevo ticket
      break;
    case '2':
      asignar_prioridad(lista_tickets) ;// Cambia la prioridad de un ticket según quiera el usuario
      break;
    case '3':
      mostrar_lista_tickets(lista_tickets);// Muestra tickets pendientes
      break;
    case '4':
      procesarTickets(lista_tickets);// Procesa el siguiente ticket según prioridad y hora
      break;
    case '5':
      buscar_Ticket(lista_tickets);// Busca un ticket por su ID
      break;
    case '6':
      puts("Saliendo del sistema de gestión de tickets...");
      break;
    default:
      puts("Opción no válida. Por favor, intente de nuevo.");
    }
  } while (opcion != '6');// Solo en caso de que la opción sea la 6 sale del bucle y finaliza el programa
  // Libera la memoria de todos los tickets
  tipoTicket *ticket = list_first(lista_tickets);
  while (ticket != NULL) {
    list_popCurrent(lista_tickets);
    // Libera cada ticket para evitar fugas de memoria
    free(ticket);
    ticket = list_first(lista_tickets);
  }
  // Libera la estructura de la lista
  list_clean(lista_tickets);
  // Libera el puntero de la lista
  free(lista_tickets) ;
  return 0;
}