#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/http_types.h"
#include "../include/request_queue.h"

typedef struct QNode {
  struct QNode *next;
  int client_socket_fd;
} QNode;

Request_Queue *createQueue() {
  Request_Queue *queue = (Request_Queue *)malloc(sizeof(Request_Queue));
  if (queue == NULL) {
    perror("request_queue malloc");
    exit(1);
  }
  queue->head = NULL;
  queue->tail = NULL;
  queue->len = 0;
  pthread_mutex_init(&queue->mutex, NULL);
  pthread_mutex_init(&queue->condition_mutex, NULL);
  pthread_cond_init(&queue->not_empty, NULL);
  return queue;
}

QNode *createNode(int client_socket_fd) {
  QNode *newNode = (QNode *)malloc(sizeof(QNode));
  if (newNode == NULL) {
    perror("queue new node");
    exit(1);
  }
  newNode->client_socket_fd = client_socket_fd;
  newNode->next = NULL;
  return newNode;
}

void enque(Request_Queue *queue, int client_socket_fd) {
  // pthread_mutex_lock(&queue->mutex);
  QNode *newNode = createNode(client_socket_fd);

  if (queue->len == 0) {
    queue->head = queue->tail = newNode;
    queue->len++;
	return;
  }

  queue->len++;
  queue->tail->next = newNode;
  queue->tail = newNode;
}

int deque(Request_Queue *queue) {

  QNode *temp = queue->head;
  int client_socket_fd = temp->client_socket_fd;
  queue->head = queue->head->next;
  if (queue->head == NULL) {
    queue->tail = NULL;
  }
  free(temp);
  queue->len--;

  // pthread_cond_broadcast(&queue->not_empty);
  // pthread_mutex_unlock(&queue->mutex);

  return client_socket_fd;
}

void monitor_queue(Request_Queue *queue) {
  QNode *current = queue->head;
  int counter = 0;

  while (current != NULL) {
    printf("Q[%d] %d\n", counter, current->client_socket_fd);
    current = current->next;
    counter++;
  }
}
