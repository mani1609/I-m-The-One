#include<pthread.h>

struct station {
  int O_pssgs; // in station waiting passengers
  int I_pssgs; // in train passengers 
  pthread_mutex_t lock;
  pthread_cond_t train_arrived_cond;
  pthread_cond_t pssgs_seated_cond;
  pthread_cond_t train_is_full_cond;
};
/**
  Initializes all the mutexes and condition-variables.
*/
void station_init(struct station *station)
{
  station->O_pssgs = 0;
  station->I_pssgs = 0;
  pthread_mutex_init(&(station->lock), NULL);
  pthread_cond_init(&(station->train_arrived_cond), NULL);
  pthread_cond_init(&(station->pssgs_seated_cond), NULL);
  pthread_cond_init(&(station->train_is_full_cond), NULL);
}

/**
Loads the train with passengers. When a passenger robot arrives in a station, it first invokes this function. 
The function must not return until the train is satisfactorily loaded.
Params:
  station: current station pointer
  count: indicates how many seats are available on the train
*/
void station_load_train(struct station *station, int count)
{
  // Enter critical region
  pthread_mutex_lock(&(station->lock));

  while ((station->O_pssgs > 0) && (count > 0)){
    pthread_cond_signal(&(station->train_arrived_cond));
  	count--;
  	pthread_cond_wait(&(station-> pssgs _seated_cond), &(station->lock));
  }

  if (station->I_pssgs > 0)
  	pthread_cond_wait(&(station->train_is_full_cond), &(station->lock));

  // Leave critical region
  pthread_mutex_unlock(&(station->lock));
}

/**
function must not return until a train is in the station and there are enough free seats on
the train for this passenger. Once this function returns, the passenger robot will move the
passenger on board the train and into a seat.
Once the passenger is seated, it will call the function: station_on_board
Params:
  station: current station pointer*/

void station_wait_for_train(struct station *station)
{
  pthread_mutex_lock(&(station->lock));

  station->O_pssgs++;
  pthread_cond_wait(&(station->train_arrived_cond), &(station->lock));
  station->O_pssgs--;
  station->I_pssgs++;

  pthread_mutex_unlock(&(station->lock));

  pthread_cond_signal(&(station-> pssgs _seated_cond));
}

/** function lets the train know that it's on board.
Params:
  stattion: current station pointer*/
void station_on_board(struct station *station)
{
  pthread_mutex_lock(&(station->lock));

  station->I_pssgs--;

  pthread_mutex_unlock(&(station->lock));

  if (station->I_pssgs == 0)
  	pthread_cond_broadcast(&(station->train_is_full_cond));

