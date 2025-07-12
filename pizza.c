#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "p3220242-p3220189-p3120203-pizza.h"

//Statikh arxikopoihsh threads - Elegxos kai sygxronismos
pthread_mutex_t order_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t order_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t chef_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t chef_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t kitchen_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t kitchen_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t driver_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t driver_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t display_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t income_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t performance_mutex = PTHREAD_MUTEX_INITIALIZER;

void *process_order(void *arg) {
    Order *order = (Order *)arg;
    int id = order->id;
    int pizzas = order->pizzas;
    int *types = order->types;
    time_t start_time = order->start_time;
    time_t end_time, cooling_start_time, cooling_end_time;
    double local_income = 0;
    int local_sales_m = 0, local_sales_p = 0 , local_sales_s = 0;

    // Eksyphrethsh kai elegxos diathesimou thlefwnhth
    pthread_mutex_lock(&order_mutex);
    while (available_tel == 0) {
        pthread_cond_wait(&order_cond, &order_mutex);
    }
    available_tel--;
    pthread_mutex_unlock(&order_mutex);

    // Xrewsh kai elegxos an ayth apotyxei me pithanothta Pfail
    sleep(get_random(Tpaymentlow, Tpaymenthigh));
    if ((double) rand() / RAND_MAX < Pfail) {
        pthread_mutex_lock(&display_mutex);
        printf("Order number %d failed.\n", id);
        pthread_mutex_unlock(&display_mutex);
        pthread_mutex_lock(&order_mutex);
        available_tel++;
        pthread_cond_signal(&order_cond);
        pthread_mutex_unlock(&order_mutex);
        pthread_mutex_lock(&performance_mutex);
        failed_orders++;
        pthread_mutex_unlock(&performance_mutex);
        return NULL;
    }

    for (int i = 0; i < pizzas; i++) {
        if (types[i] == 0) {
            local_income += Cm;
            local_sales_m++;
        } else if (types[i] == 1) {
            local_income += Cp;
            local_sales_p++;
        } else if (types[i] == 2) {
            local_income += Cs;
            local_sales_s++;
        }
    }

    pthread_mutex_lock(&order_mutex);
    printf("Order number %d approved !\n", id);
    pthread_mutex_unlock(&order_mutex);

    pthread_mutex_lock(&order_mutex);
    available_tel++;
    pthread_cond_signal(&order_cond);
    pthread_mutex_unlock(&order_mutex);

    // Eksyphrethsh apo chef
    pthread_mutex_lock(&chef_mutex);
    while (available_cook == 0) {
        pthread_cond_wait(&chef_cond, &chef_mutex);
    }
    available_cook--;
    pthread_mutex_unlock(&chef_mutex);

    sleep(Tprep * pizzas);

    pthread_mutex_lock(&chef_mutex);
    available_cook++;
    pthread_cond_signal(&chef_cond);
    pthread_mutex_unlock(&chef_mutex);

    // Xrhsh fournwn
    pthread_mutex_lock(&kitchen_mutex);
    while (available_oven < pizzas) {
        pthread_cond_wait(&kitchen_cond, &kitchen_mutex);
    }
    available_oven -= pizzas;
    pthread_mutex_unlock(&kitchen_mutex);

    sleep(Tbake);

    cooling_start_time = time(NULL);

    pthread_mutex_lock(&kitchen_mutex);
    available_oven += pizzas;
    pthread_cond_signal(&kitchen_cond);
    pthread_mutex_unlock(&kitchen_mutex);

    // Eksyphrethsh apo odhgo-dianomea
    pthread_mutex_lock(&driver_mutex);
    while (available_deliverer == 0) {
        pthread_cond_wait(&driver_cond, &driver_mutex);
    }
    available_deliverer--;
    pthread_mutex_unlock(&driver_mutex);

    sleep(Tpack * pizzas);
    sleep(get_random(Tdellow, Tdelhigh));

    end_time = time(NULL);
    cooling_end_time = end_time;

    pthread_mutex_lock(&driver_mutex);
    available_deliverer++;
    pthread_cond_signal(&driver_cond);
    pthread_mutex_unlock(&driver_mutex);

    pthread_mutex_lock(&display_mutex);
    printf("Order %d has been prepared in %d minutes.\n", id, (int)(end_time - start_time));
    printf("Order %d has been delivered in %d minutes.\n", id, (int)((end_time - start_time)+Tpack));
    pthread_mutex_unlock(&display_mutex);

    pthread_mutex_lock(&income_mutex);
    income += local_income;
    sales_m += local_sales_m;
    sales_p += local_sales_p;
    sales_s += local_sales_s;
    pthread_mutex_unlock(&income_mutex);

    // Pwlhseis, esoda, statistika paraggelias
    pthread_mutex_lock(&performance_mutex);
    success_orders++;
    time_t service_time = end_time - start_time;
    time_t cooling_time = cooling_end_time - cooling_start_time;
    total_service_time += service_time;
    if (service_time > max_service_time) {
        max_service_time = service_time;
    }
    total_cooling_time += cooling_time;
    if (cooling_time > max_cooling_time) {
        max_cooling_time = cooling_time;
    }
    pthread_mutex_unlock(&performance_mutex);

    return NULL;
}


int get_random(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Elegxos orthis syntakshs
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Ncust> <Seed>\n", argv[0]);
        exit(EXIT_FAILURE);// minima lathous
    }

    int Ncust = atoi(argv[1]);// arithmos pelatwn
    int seed = atoi(argv[2]);// sporos gia arxikopoihsh srand
    srand(seed);

    pthread_t threads[Ncust];// apothikeysh nhmatwn
    Order orders[Ncust];// apothikeysh nhmatwn

    for (int i = 0; i < Ncust; i++) {
        orders[i].id = i + 1;
        orders[i].pizzas = get_random(Norderlow, Norderhigh);
        orders[i].types = malloc(orders[i].pizzas * sizeof(int));
        orders[i].start_time = time(NULL);
        for (int j = 0; j < orders[i].pizzas; j++) {
            double rand_val = (double) rand() / RAND_MAX;
            if (rand_val < Pm) {
                orders[i].types[j] = 0; // Margarita
            } else if (rand_val < Pm + Pp) {
                orders[i].types[j] = 1; // Peperoni
            } else {
                orders[i].types[j] = 2; // Special
            }
        }
        pthread_create(&threads[i], NULL, process_order, (void *)&orders[i]);
        sleep(get_random(Torderlow, Torderhigh));
    }

    for (int i = 0; i < Ncust; i++) {
        pthread_join(threads[i], NULL);
        free(orders[i].types);
    }
    printf("*******************************\n");
    printf("F i n a l   S t a t i s t i c s\n");
    printf("*******************************\n\n");
    printf("Total income : %d euros\n", (int)income);
    printf("Number of Pizza Margaritas : %d\n", sales_m);
    printf("Number of Pizza Peperonis  : %d\n", sales_p);
    printf("Number of Pizza Special: %d\n", sales_s);
    printf(":-) Succeeded orders : %d\n", success_orders);
    printf(":-( Failed orders    : %d\n", failed_orders);
    if (success_orders > 0) {
        printf("Average service time: %d minutes\n", (int)total_service_time /success_orders);
        printf("Maximum service time: %d minutes\n", (int)max_service_time );
        printf("Average cooling time: %d minutes\n", (int)total_cooling_time /success_orders);
        printf("Maximum cooling time: %d minutes\n", (int)max_cooling_time);
    }

    return 0;
}
