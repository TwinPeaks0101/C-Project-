#ifndef pizza_header
#define pizza_header

#include <pthread.h>
#include <time.h>

//statheres, parametroi, arithmos proswpikou, xronoi, pithanothtes, kostos
#define Ntel 2
#define Ncook 2
#define Noven 10
#define Ndeliverer 10
#define Torderlow 1
#define Torderhigh 5
#define Norderlow 1
#define Norderhigh 5
#define Pm 0.35
#define Pp 0.25
#define Ps 0.40
#define Tpaymentlow 1
#define Tpaymenthigh 3
#define Pfail 0.05
#define Cm 10
#define Cp 11
#define Cs 12
#define Tprep 1
#define Tbake 10
#define Tpack 1
#define Tdellow 5
#define Tdelhigh 15

typedef struct {
    int id;
    int pizzas;
    int *types;
    time_t start_time;
} Order;

// ekswterikoi metavlhtoi typoi pthread_mutex_t, pthread_cond_t gia sygxronismo kai asfaleia nhmatwn
pthread_mutex_t order_mutex;
pthread_cond_t order_cond;
pthread_mutex_t chef_mutex;
pthread_cond_t chef_cond;
pthread_mutex_t kitchen_mutex;
pthread_cond_t kitchen_cond;
pthread_mutex_t driver_mutex;
pthread_cond_t driver_cond;
pthread_mutex_t display_mutex;
pthread_mutex_t income_mutex;
pthread_mutex_t performance_mutex;


// diathesimothta proswpikou, eksoplismou, esoda , pwlhseis ana eidos pitsas
// epityxhmenes kai apotyxhmenes paraggelies, xronoi eksyphrethshs kai psykshs- statistika
// arxikopoihsh katametrhtvn kai statistikwn
unsigned int available_tel = Ntel;
unsigned int available_cook = Ncook;
unsigned int available_oven = Noven;
unsigned int available_deliverer = Ndeliverer;
double income = 0;
int sales_m = 0;
int sales_p = 0;
int sales_s = 0;
int success_orders = 0;
int failed_orders = 0;
time_t max_service_time = 0;
time_t total_service_time = 0;
time_t max_cooling_time = 0;
time_t total_cooling_time = 0;

// dhlwseis synarthsewn
int get_random(int min , int max);//epistrofi tyxaiou arithmou sto [min,max]
void *process_order(void *arg);//epeksergasia paraggelias se nhma

#endif // pizza_header
