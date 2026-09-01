#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FLIGHT_COUNT 14
#define MAX_BOOKINGS 50
#define TEXT_SIZE 60
#define CODE_SIZE 8

typedef struct {
    char flightNo[10];
    char airline[TEXT_SIZE];
    char from[TEXT_SIZE];
    char to[TEXT_SIZE];
    char date[11];
    char departTime[6];
    char arriveTime[6];
    char terminal[5];
    char gate[6];
    char aircraft[TEXT_SIZE];
    char status[TEXT_SIZE];
    int durationMinutes;
    int delayMinutes;
    int arriveNextDay;
    int economyPrice;
    int businessPrice;
    int economySeats;
    int businessSeats;
} Flight;

typedef struct {
    char name[TEXT_SIZE];
    char phone[TEXT_SIZE];
    char email[TEXT_SIZE];
    char id[TEXT_SIZE];
} Passenger;

typedef struct {
    char bookingCode[CODE_SIZE];
    Passenger passenger;
    int flightIndex;
    int tickets;
    int cabinClass;
    int totalPrice;
    int active;
} Booking;

void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

void readLine(char text[], int size) {
    if (fgets(text, size, stdin) != NULL) {
        text[strcspn(text, "\n")] = '\0';
    }
}

int readIntInRange(const char prompt[], int min, int max) {
    int value;
    int result;

    while (1) {
        printf("%s", prompt);
        result = scanf("%d", &value);
        clearInputBuffer();

        if (result == 1 && value >= min && value <= max) {
            return value;
        }

        printf("Invalid input. Please enter a number from %d to %d.\n", min, max);
    }
}

int timeToMinutes(const char timeText[]) {
    int hour, minute;
    if (sscanf(timeText, "%d:%d", &hour, &minute) != 2) {
        return -1;
    }

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return -1;
    }

    return hour * 60 + minute;
}

void minutesToTime(int minutes, char output[], int *nextDay) {
    int hour;
    int minute;

    if (minutes >= 24 * 60) {
        *nextDay = 1;
    } else {
        *nextDay = 0;
    }

    minutes %= 24 * 60;
    hour = minutes / 60;
    minute = minutes % 60;
    output[0] = (char)('0' + hour / 10);
    output[1] = (char)('0' + hour % 10);
    output[2] = ':';
    output[3] = (char)('0' + minute / 10);
    output[4] = (char)('0' + minute % 10);
    output[5] = '\0';
}

int textContainsIgnoreCase(const char text[], const char keyword[]) {
    int i, j;
    int textLength = (int)strlen(text);
    int keyLength = (int)strlen(keyword);

    if (keyLength == 0) {
        return 1;
    }

    for (i = 0; i <= textLength - keyLength; i++) {
        for (j = 0; j < keyLength; j++) {
            if (tolower((unsigned char)text[i + j]) !=
                tolower((unsigned char)keyword[j])) {
                break;
            }
        }

        if (j == keyLength) {
            return 1;
        }
    }

    return 0;
}

void makeTodayDate(char dateText[]) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    if (local == NULL) {
        strcpy(dateText, "2026-08-22");
        return;
    }

    strftime(dateText, 11, "%Y-%m-%d", local);
}

void swapFlights(Flight *a, Flight *b) {
    Flight temp = *a;
    *a = *b;
    *b = temp;
}

void sortFlightsByTime(Flight flights[], int count) {
    int i, j;
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (timeToMinutes(flights[j].departTime) >
                timeToMinutes(flights[j + 1].departTime)) {
                swapFlights(&flights[j], &flights[j + 1]);
            }
        }
    }
}

void generateFlights(Flight flights[], int count) {
    const char *cities[] = {
        "Taipei", "Tokyo", "Seoul", "Bangkok", "Singapore",
        "Hong Kong", "Osaka", "Manila", "Kuala Lumpur", "Ho Chi Minh"
    };
    const char *airlines[] = {
        "Cloud Taiwan", "Pacific Star", "Asia Link", "Sky Harbor"
    };
    const char *aircrafts[] = {
        "Airbus A320", "Airbus A321neo", "Boeing 737-800", "Boeing 787-9"
    };
    const char *statuses[] = {
        "On Time", "On Time", "On Time", "Delayed", "Boarding", "Cancelled"
    };
    char today[11];
    int cityCount = sizeof(cities) / sizeof(cities[0]);
    int usedTimes[24 * 60] = {0};
    int usedFlightNumbers[1000] = {0};
    int i;

    srand((unsigned int)time(NULL));
    makeTodayDate(today);

    for (i = 0; i < count; i++) {
        int fromIndex, toIndex, hour, minute, departMinutes, flightNumber;
        int statusIndex, arriveMinutes;

        do {
            fromIndex = rand() % cityCount;
            toIndex = rand() % cityCount;
        } while (fromIndex == toIndex);

        do {
            hour = 6 + rand() % 17;
            minute = (rand() % 4) * 15;
            departMinutes = hour * 60 + minute;
        } while (usedTimes[departMinutes]);

        usedTimes[departMinutes] = 1;
        statusIndex = rand() % (sizeof(statuses) / sizeof(statuses[0]));

        do {
            flightNumber = 100 + rand() % 900;
        } while (usedFlightNumbers[flightNumber]);
        usedFlightNumbers[flightNumber] = 1;

        snprintf(flights[i].flightNo, sizeof(flights[i].flightNo), "CT%03d", flightNumber);
        strcpy(flights[i].airline, airlines[rand() % (sizeof(airlines) / sizeof(airlines[0]))]);
        strcpy(flights[i].from, cities[fromIndex]);
        strcpy(flights[i].to, cities[toIndex]);
        strcpy(flights[i].date, today);
        flights[i].departTime[0] = (char)('0' + hour / 10);
        flights[i].departTime[1] = (char)('0' + hour % 10);
        flights[i].departTime[2] = ':';
        flights[i].departTime[3] = (char)('0' + minute / 10);
        flights[i].departTime[4] = (char)('0' + minute % 10);
        flights[i].departTime[5] = '\0';
        snprintf(flights[i].terminal, sizeof(flights[i].terminal), "T%d", 1 + rand() % 3);
        snprintf(flights[i].gate, sizeof(flights[i].gate), "%c%d", 'A' + rand() % 6, 1 + rand() % 20);
        strcpy(flights[i].aircraft, aircrafts[rand() % (sizeof(aircrafts) / sizeof(aircrafts[0]))]);
        strcpy(flights[i].status, statuses[statusIndex]);
        flights[i].durationMinutes = 90 + (rand() % 13) * 15;
        flights[i].delayMinutes = strcmp(flights[i].status, "Delayed") == 0 ? 15 + (rand() % 4) * 15 : 0;
        arriveMinutes = departMinutes + flights[i].durationMinutes + flights[i].delayMinutes;
        minutesToTime(arriveMinutes, flights[i].arriveTime, &flights[i].arriveNextDay);
        flights[i].economyPrice = 2500 + (rand() % 16) * 500;
        flights[i].businessPrice = flights[i].economyPrice + 4500 + (rand() % 8) * 500;
        flights[i].economySeats = strcmp(flights[i].status, "Cancelled") == 0 ? 0 : 20 + rand() % 121;
        flights[i].businessSeats = strcmp(flights[i].status, "Cancelled") == 0 ? 0 : 2 + rand() % 19;
    }

    sortFlightsByTime(flights, count);
}

void printFlightHeader(void) {
    printf("No Flight Route                Dep   Arr     T/G   St     E/B    Price\n");
    printf("----------------------------------------------------------------------\n");
}

void printFlight(const Flight *flight, int number) {
    char route[TEXT_SIZE * 2 + 4];
    char arrival[10];
    char terminalGate[12];
    char shortStatus[7];

    snprintf(route, sizeof(route), "%s->%s", flight->from, flight->to);
    snprintf(arrival, sizeof(arrival), "%s%s", flight->arriveTime, flight->arriveNextDay ? "+1" : "");
    snprintf(terminalGate, sizeof(terminalGate), "%s/%s", flight->terminal, flight->gate);

    if (strcmp(flight->status, "Cancelled") == 0) {
        strcpy(shortStatus, "Cxl");
    } else if (strcmp(flight->status, "Delayed") == 0) {
        strcpy(shortStatus, "Delay");
    } else if (strcmp(flight->status, "Boarding") == 0) {
        strcpy(shortStatus, "Board");
    } else {
        strcpy(shortStatus, "OnTime");
    }

    printf("%-2d %-6s %-20.20s %-5s %-7s %-5s %-6s %3d/%-2d %5d\n",
           number,
           flight->flightNo,
           route,
           flight->departTime,
           arrival,
           terminalGate,
           shortStatus,
           flight->economySeats,
           flight->businessSeats,
           flight->economyPrice);
}

void showFlightDetails(const Flight *flight) {
    printf("\n=== Flight Details ===\n");
    printf("Flight      : %s (%s)\n", flight->flightNo, flight->airline);
    printf("Date        : %s\n", flight->date);
    printf("Route       : %s -> %s\n", flight->from, flight->to);
    printf("Departure   : %s\n", flight->departTime);
    printf("Arrival     : %s%s\n", flight->arriveTime, flight->arriveNextDay ? " next day" : "");
    printf("Duration    : %d hr %d min\n", flight->durationMinutes / 60, flight->durationMinutes % 60);
    printf("Terminal    : %s\n", flight->terminal);
    printf("Gate        : %s\n", flight->gate);
    printf("Aircraft    : %s\n", flight->aircraft);
    printf("Status      : %s", flight->status);
    if (flight->delayMinutes > 0) {
        printf(" (%d min delay)", flight->delayMinutes);
    }
    printf("\n");
    printf("Economy     : NT$%d, %d seats left, baggage 1 x 23kg\n",
           flight->economyPrice, flight->economySeats);
    printf("Business    : NT$%d, %d seats left, baggage 2 x 32kg\n",
           flight->businessPrice, flight->businessSeats);
}

void showAllFlights(Flight flights[], int count) {
    int i;
    printf("\n=== Current Flights ===\n");
    printFlightHeader();
    for (i = 0; i < count; i++) {
        printFlight(&flights[i], i + 1);
    }
}

int searchFlights(Flight flights[], int count, int matchedIndexes[]) {
    char fromKeyword[TEXT_SIZE];
    char toKeyword[TEXT_SIZE];
    char wantedTime[20];
    int wantedMinutes;
    int i, matchedCount = 0;

    printf("\nFrom city keyword (press Enter for any): ");
    readLine(fromKeyword, TEXT_SIZE);

    printf("To city keyword (press Enter for any): ");
    readLine(toKeyword, TEXT_SIZE);

    while (1) {
        printf("Earliest departure time HH:MM (press Enter for any): ");
        readLine(wantedTime, sizeof(wantedTime));

        if (strlen(wantedTime) == 0) {
            wantedMinutes = -1;
            break;
        }

        wantedMinutes = timeToMinutes(wantedTime);
        if (wantedMinutes != -1) {
            break;
        }

        printf("Invalid time format. Example: 09:30\n");
    }

    printf("\n=== Search Results ===\n");
    printFlightHeader();

    for (i = 0; i < count; i++) {
        int departMinutes = timeToMinutes(flights[i].departTime);
        int matchesFrom = textContainsIgnoreCase(flights[i].from, fromKeyword);
        int matchesTo = textContainsIgnoreCase(flights[i].to, toKeyword);
        int matchesTime = wantedMinutes == -1 || departMinutes >= wantedMinutes;

        if (matchesFrom && matchesTo && matchesTime) {
            matchedIndexes[matchedCount] = i;
            printFlight(&flights[i], matchedCount + 1);
            matchedCount++;
        }
    }

    if (matchedCount == 0) {
        printf("No matching flights were found.\n");
    }

    return matchedCount;
}

int findBookingByCode(Booking bookings[], int bookingCount, const char code[]) {
    int i;
    for (i = 0; i < bookingCount; i++) {
        if (bookings[i].active && strcmp(bookings[i].bookingCode, code) == 0) {
            return i;
        }
    }

    return -1;
}

void generateBookingCode(char code[], Booking bookings[], int bookingCount) {
    const char alphabet[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    int unique;
    int i;

    do {
        unique = 1;
        for (i = 0; i < 6; i++) {
            code[i] = alphabet[rand() % (sizeof(alphabet) - 1)];
        }
        code[6] = '\0';

        if (findBookingByCode(bookings, bookingCount, code) != -1) {
            unique = 0;
        }
    } while (!unique);
}

int getAvailableSeats(const Flight *flight, int cabinClass) {
    return cabinClass == 1 ? flight->economySeats : flight->businessSeats;
}

int getCabinPrice(const Flight *flight, int cabinClass) {
    return cabinClass == 1 ? flight->economyPrice : flight->businessPrice;
}

void reduceSeats(Flight *flight, int cabinClass, int tickets) {
    if (cabinClass == 1) {
        flight->economySeats -= tickets;
    } else {
        flight->businessSeats -= tickets;
    }
}

void returnSeats(Flight *flight, int cabinClass, int tickets) {
    if (cabinClass == 1) {
        flight->economySeats += tickets;
    } else {
        flight->businessSeats += tickets;
    }
}

void printBooking(const Booking *booking, const Flight *flight) {
    printf("\n=== Booking Information ===\n");
    printf("Booking code : %s\n", booking->bookingCode);
    printf("Passenger    : %s\n", booking->passenger.name);
    printf("Phone        : %s\n", booking->passenger.phone);
    printf("Email        : %s\n", booking->passenger.email);
    printf("ID/Passport  : %s\n", booking->passenger.id);
    printf("Flight       : %s (%s)\n", flight->flightNo, flight->airline);
    printf("Route        : %s -> %s\n", flight->from, flight->to);
    printf("Date/Time    : %s %s\n", flight->date, flight->departTime);
    printf("Terminal/Gate: %s / %s\n", flight->terminal, flight->gate);
    printf("Cabin        : %s\n", booking->cabinClass == 1 ? "Economy" : "Business");
    printf("Tickets      : %d\n", booking->tickets);
    printf("Total        : NT$%d\n", booking->totalPrice);
    printf("Status       : Confirmed\n");
}

void bookFlight(Flight flights[], int flightCount, Booking bookings[], int *bookingCount) {
    int matchedIndexes[FLIGHT_COUNT];
    int matchedCount;
    int choice;
    int flightIndex;
    int cabinClass;
    int availableSeats;
    int tickets;
    int unitPrice;
    Booking booking;

    if (*bookingCount >= MAX_BOOKINGS) {
        printf("\nBooking storage is full. Cannot create more bookings.\n");
        return;
    }

    matchedCount = searchFlights(flights, flightCount, matchedIndexes);
    if (matchedCount == 0) {
        return;
    }

    choice = readIntInRange("\nChoose a flight from the search results: ", 1, matchedCount);
    flightIndex = matchedIndexes[choice - 1];

    showFlightDetails(&flights[flightIndex]);

    if (strcmp(flights[flightIndex].status, "Cancelled") == 0) {
        printf("\nThis flight has been cancelled and cannot be booked.\n");
        return;
    }

    printf("\nCabin class\n");
    printf("1. Economy  - NT$%d, seats left %d\n",
           flights[flightIndex].economyPrice, flights[flightIndex].economySeats);
    printf("2. Business - NT$%d, seats left %d\n",
           flights[flightIndex].businessPrice, flights[flightIndex].businessSeats);
    cabinClass = readIntInRange("Choose cabin class: ", 1, 2);

    availableSeats = getAvailableSeats(&flights[flightIndex], cabinClass);
    if (availableSeats <= 0) {
        printf("No seats left in this cabin.\n");
        return;
    }

    tickets = readIntInRange("How many tickets do you want? ", 1, availableSeats);

    printf("\nPassenger name: ");
    readLine(booking.passenger.name, TEXT_SIZE);

    printf("Phone number: ");
    readLine(booking.passenger.phone, TEXT_SIZE);

    printf("Email: ");
    readLine(booking.passenger.email, TEXT_SIZE);

    printf("ID or passport number: ");
    readLine(booking.passenger.id, TEXT_SIZE);

    generateBookingCode(booking.bookingCode, bookings, *bookingCount);
    booking.flightIndex = flightIndex;
    booking.tickets = tickets;
    booking.cabinClass = cabinClass;
    unitPrice = getCabinPrice(&flights[flightIndex], cabinClass);
    booking.totalPrice = unitPrice * tickets;
    booking.active = 1;

    reduceSeats(&flights[flightIndex], cabinClass, tickets);
    bookings[*bookingCount] = booking;
    (*bookingCount)++;

    printBooking(&booking, &flights[flightIndex]);
    printf("\nPlease keep your booking code for check-in or cancellation.\n");
}

void queryBooking(Flight flights[], Booking bookings[], int bookingCount) {
    char code[CODE_SIZE];
    int index;

    printf("\nEnter booking code: ");
    readLine(code, CODE_SIZE);

    index = findBookingByCode(bookings, bookingCount, code);
    if (index == -1) {
        printf("Booking not found.\n");
        return;
    }

    printBooking(&bookings[index], &flights[bookings[index].flightIndex]);
}

void cancelBooking(Flight flights[], Booking bookings[], int bookingCount) {
    char code[CODE_SIZE];
    int index;
    int confirm;

    printf("\nEnter booking code to cancel: ");
    readLine(code, CODE_SIZE);

    index = findBookingByCode(bookings, bookingCount, code);
    if (index == -1) {
        printf("Booking not found.\n");
        return;
    }

    printBooking(&bookings[index], &flights[bookings[index].flightIndex]);
    confirm = readIntInRange("\nConfirm cancellation? 1.Yes  2.No: ", 1, 2);

    if (confirm == 1) {
        returnSeats(&flights[bookings[index].flightIndex],
                    bookings[index].cabinClass,
                    bookings[index].tickets);
        bookings[index].active = 0;
        printf("Booking %s has been cancelled. Seats were released.\n", code);
    } else {
        printf("Cancellation aborted.\n");
    }
}

void showMenu(void) {
    printf("\n=== Flight Query and Booking System ===\n");
    printf("1. Show all flights\n");
    printf("2. Search flights\n");
    printf("3. Book a flight\n");
    printf("4. Check booking\n");
    printf("5. Cancel booking\n");
    printf("6. Exit\n");
}

int main(void) {
    Flight flights[FLIGHT_COUNT];
    Booking bookings[MAX_BOOKINGS];
    int bookingCount = 0;
    int matchedIndexes[FLIGHT_COUNT];
    int choice;
    int detailChoice;
    int matchedCount;

    generateFlights(flights, FLIGHT_COUNT);

    do {
        showMenu();
        choice = readIntInRange("Choose an option: ", 1, 6);

        switch (choice) {
            case 1:
                showAllFlights(flights, FLIGHT_COUNT);
                detailChoice = readIntInRange("\nView details? Enter flight No. or 0 to return: ",
                                              0, FLIGHT_COUNT);
                if (detailChoice != 0) {
                    showFlightDetails(&flights[detailChoice - 1]);
                }
                break;
            case 2:
                matchedCount = searchFlights(flights, FLIGHT_COUNT, matchedIndexes);
                if (matchedCount > 0) {
                    detailChoice = readIntInRange("\nView details? Enter result No. or 0 to return: ",
                                                  0, matchedCount);
                    if (detailChoice != 0) {
                        showFlightDetails(&flights[matchedIndexes[detailChoice - 1]]);
                    }
                }
                break;
            case 3:
                bookFlight(flights, FLIGHT_COUNT, bookings, &bookingCount);
                break;
            case 4:
                queryBooking(flights, bookings, bookingCount);
                break;
            case 5:
                cancelBooking(flights, bookings, bookingCount);
                break;
            case 6:
                printf("Goodbye.\n");
                break;
        }
    } while (choice != 6);

    return 0;
}
