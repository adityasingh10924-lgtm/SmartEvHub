#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include <string>

class Database {
private:
    sqlite3* db;

public:
    Database();
    ~Database();

    bool connect(const std::string& databasePath);
    bool initializeSchema(const std::string& schemaPath);
    void disconnect();
    bool registerUser(
        const std::string& name,
        const std::string& email,
        const std::string& password
    );
    
    bool loginUser(
       const std::string& email,
       const std::string& password
    );

    int getLoggedInUserID(
    const std::string& email,
    const std::string& password
     );

    bool addVehicle(
       int userID,
       const std::string& vehicleNumber,
       const std::string& vehicleType,
       double batteryCapacity
    );

     void getVehicles(int userID);

    bool addChargingStation(
    const std::string& stationName,
    const std::string& location,
    int totalSlots,
    int availableSlots,
    const std::string& status
    );

    void getChargingStations();

    bool createChargingRequest(
    int userID,
    int vehicleID,
    int stationID
    );
     
    void getChargingRequests();

    bool updateRequestStatus(
    int requestID,
    const std::string& newStatus
    );

    bool completeChargingRequest(
        int requestID,
        double energyCharged
    );

    void getTransactions(int userID);
};

#endif