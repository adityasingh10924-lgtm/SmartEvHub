#include "Database.h"
#include <iostream>
#include <string>

using namespace std;

void showMenu()
{
    cout << "\n========== SMART EV HUB ==========\n";
    cout << "1. Add Vehicle\n";
    cout << "2. View My Vehicles\n";
    cout << "3. View Charging Stations\n";
    cout << "4. Create Charging Request\n";
    cout << "5. View Charging Requests\n";
    cout << "6. Complete Charging Request\n";
    cout << "7. View My Transactions\n";
    cout << "8. Logout\n";
    cout << "==================================\n";
    cout << "Enter choice: ";
}

int main()
{
    Database database;

    if (!database.connect("../database/evhub.db"))
    {
        return 1;
    }

    string email;
    string password;

    cout << "\n========== EV HUB LOGIN ==========\n";
    cout << "Email: ";
    cin >> email;

    cout << "Password: ";
    cin >> password;

    int userID = database.getLoggedInUserID(email, password);

    if (userID == -1)
    {
        cout << "\nLogin failed!\n";
        return 0;
    }

    cout << "\nLogin successful!\n";
    cout << "Logged in User ID: " << userID << "\n";

    int choice;

    while (true)
    {
        showMenu();
        cin >> choice;

        if (choice == 1)
{
    string vehicleNumber;
    string vehicleType;
    double batteryCapacity;

    cout << "\n--- Add Vehicle ---\n";

    cin.ignore();

    cout << "Vehicle Number: ";
    getline(cin, vehicleNumber);

    cout << "Vehicle Type: ";
    getline(cin, vehicleType);

    cout << "Battery Capacity (kWh): ";
    cin >> batteryCapacity;

    database.addVehicle(
        userID,
        vehicleNumber,
        vehicleType,
        batteryCapacity
    );
}
         else if (choice == 2)
{
    database.getVehicles(userID);
}


         else if (choice == 3)
{
    database.getChargingStations();
}

        else if (choice == 4)
        {
            int vehicleID;
            int stationID;

            cout << "\n--- Create Charging Request ---\n";

            cout << "Vehicle ID: ";
            cin >> vehicleID;

            cout << "Station ID: ";
            cin >> stationID;

            database.createChargingRequest(
                userID,
                vehicleID,
                stationID
            );
        }

         else if (choice == 5)
{
    database.getChargingRequests();
}

        else if (choice == 6)
        {
            int requestID;
double energyCharged;

cout << "\n--- Complete Charging Request ---\n";

cout << "Request ID: ";
cin >> requestID;

cout << "Energy Charged (kWh): ";
cin >> energyCharged;

database.completeChargingRequest(
    requestID,
    energyCharged
);
        }
         else if (choice == 7)
{
    database.getTransactions(userID);
}

        else if (choice == 8)
        {
            cout << "\nLogged out successfully.\n";
            break;
        }

        else
        {
            cout << "\nInvalid choice. Please try again.\n";
        }
    }

    return 0;
}