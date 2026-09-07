#include "Database.h"
#include <iostream>
#include <fstream>
#include <sstream>

Database::Database() {
    db = nullptr;
}

Database::~Database() {
    disconnect();
}

bool Database::connect(const std::string& databasePath) {

    int result = sqlite3_open(databasePath.c_str(), &db);

    if (result != SQLITE_OK) {
        std::cerr << "Database connection failed: "
                  << sqlite3_errmsg(db) << std::endl;

        if (db != nullptr) {
            sqlite3_close(db);
            db = nullptr;
        }

        return false;
    }

    std::cout << "Database connected successfully!" << std::endl;

    return true;
}

bool Database::initializeSchema(const std::string& schemaPath) {

    std::ifstream schemaFile(schemaPath);

    if (!schemaFile.is_open()) {
        std::cerr << "Failed to open schema file: "
                  << schemaPath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << schemaFile.rdbuf();

    std::string schema = buffer.str();

    schemaFile.close();

    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        db,
        schema.c_str(),
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {

        std::cerr << "Database schema initialization failed: "
                  << (errorMessage ? errorMessage : "Unknown error")
                  << std::endl;

        if (errorMessage != nullptr) {
            sqlite3_free(errorMessage);
        }

        return false;
    }

    std::cout << "Database schema initialized successfully!"
              << std::endl;

    return true;
}
void Database::disconnect() {

    if (db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::registerUser(
    const std::string& name,
    const std::string& email,
    const std::string& password
) {

    const char* sql =
        "INSERT INTO Users (Name, Email, Password) "
        "VALUES (?, ?, ?);";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare query: "
                  << sqlite3_errmsg(db) << std::endl;

        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        name.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        2,
        email.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        3,
        password.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);

    sqlite3_finalize(statement);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to register user."
                  << std::endl;

        return false;
    }

    std::cout << "User registered successfully!" << std::endl;

    return true;
}

bool Database::loginUser(
    const std::string& email,
    const std::string& password
) {

    const char* sql =
        "SELECT Name FROM Users "
        "WHERE Email = ? AND Password = ?;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare login query: "
                  << sqlite3_errmsg(db) << std::endl;

        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        email.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        2,
        password.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW) {

        const unsigned char* name =
            sqlite3_column_text(statement, 0);

        std::cout << "Login successful!" << std::endl;
        std::cout << "Welcome, " << name << std::endl;

        sqlite3_finalize(statement);

        return true;
    }

    sqlite3_finalize(statement);

    std::cout << "Login failed!" << std::endl;

    return false;
}

bool Database::addVehicle(
    int userID,
    const std::string& vehicleNumber,
    const std::string& vehicleType,
    double batteryCapacity
) {
     if (vehicleNumber.empty()) {
    std::cout << "Vehicle number cannot be empty."
              << std::endl;
    return false;
}

if (vehicleType.empty()) {
    std::cout << "Vehicle type cannot be empty."
              << std::endl;
    return false;
}

if (batteryCapacity <= 0) {
    std::cout << "Battery capacity must be greater than 0."
              << std::endl;
    return false;
}

    const char* sql =
        "INSERT INTO Vehicles "
        "(UserID, VehicleNumber, VehicleType, BatteryCapacity) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare vehicle query: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(statement, 1, userID);

    sqlite3_bind_text(
        statement,
        2,
        vehicleNumber.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        3,
        vehicleType.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_double(
        statement,
        4,
        batteryCapacity
    );

    result = sqlite3_step(statement);

    sqlite3_finalize(statement);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to add vehicle: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::cout << "Vehicle added successfully!" << std::endl;

    return true;
}

void Database::getVehicles(int userID) {

    const char* sql =
        "SELECT VehicleID, VehicleNumber, VehicleType, BatteryCapacity "
        "FROM Vehicles "
        "WHERE UserID = ?;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to retrieve vehicles: "
                  << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(statement, 1, userID);

    std::cout << "\n--- My Vehicles ---\n";

    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {

        int vehicleID = sqlite3_column_int(statement, 0);

        const unsigned char* vehicleNumber =
            sqlite3_column_text(statement, 1);

        const unsigned char* vehicleType =
            sqlite3_column_text(statement, 2);

        double batteryCapacity =
            sqlite3_column_double(statement, 3);

        std::cout << "Vehicle ID: " << vehicleID << std::endl;
        std::cout << "Vehicle Number: " << vehicleNumber << std::endl;
        std::cout << "Vehicle Type: " << vehicleType << std::endl;
        std::cout << "Battery Capacity: "
                  << batteryCapacity << " kWh" << std::endl;
        std::cout << "-------------------------\n";
    }

    sqlite3_finalize(statement);
}

bool Database::addChargingStation(
    const std::string& stationName,
    const std::string& location,
    int totalSlots,
    int availableSlots,
    const std::string& status
) {

    const char* sql =
        "INSERT INTO ChargingStations "
        "(StationName, Location, TotalSlots, AvailableSlots, Status) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare station query: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(
        statement, 1,
        stationName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement, 2,
        location.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(statement, 3, totalSlots);
    sqlite3_bind_int(statement, 4, availableSlots);

    sqlite3_bind_text(
        statement, 5,
        status.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);

    sqlite3_finalize(statement);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to add charging station: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::cout << "Charging station added successfully!"
              << std::endl;

    return true;
}

void Database::getChargingStations() {

    const char* sql =
        "SELECT StationID, StationName, Location, "
        "TotalSlots, AvailableSlots, Status "
        "FROM ChargingStations;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to retrieve charging stations: "
                  << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "\n--- Charging Stations ---\n";

    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {

        int stationID = sqlite3_column_int(statement, 0);

        const unsigned char* stationName =
            sqlite3_column_text(statement, 1);

        const unsigned char* location =
            sqlite3_column_text(statement, 2);

        int totalSlots =
            sqlite3_column_int(statement, 3);

        int availableSlots =
            sqlite3_column_int(statement, 4);

        const unsigned char* status =
            sqlite3_column_text(statement, 5);

        std::cout << "Station ID: " << stationID << std::endl;
        std::cout << "Station Name: " << stationName << std::endl;
        std::cout << "Location: " << location << std::endl;
        std::cout << "Total Slots: " << totalSlots << std::endl;
        std::cout << "Available Slots: "
                  << availableSlots << std::endl;
        std::cout << "Status: " << status << std::endl;
        std::cout << "-------------------------\n";
    }

    sqlite3_finalize(statement);
}

bool Database::createChargingRequest(
    int userID,
    int vehicleID,
    int stationID
) {
    // Check that the vehicle exists and belongs to the logged-in user
    const char* vehicleSQL =
        "SELECT VehicleID "
        "FROM Vehicles "
        "WHERE VehicleID = ? AND UserID = ?;";

    sqlite3_stmt* vehicleStatement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        vehicleSQL,
        -1,
        &vehicleStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to check vehicle: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(vehicleStatement, 1, vehicleID);
    sqlite3_bind_int(vehicleStatement, 2, userID);

    result = sqlite3_step(vehicleStatement);

    sqlite3_finalize(vehicleStatement);

    if (result != SQLITE_ROW) {
        std::cout << "Invalid vehicle ID or vehicle does not belong to this user."
                  << std::endl;
        return false;
    }

    // Check that the station exists, is active, and has an available slot
    const char* stationSQL =
        "SELECT AvailableSlots, Status "
        "FROM ChargingStations "
        "WHERE StationID = ?;";

    sqlite3_stmt* stationStatement = nullptr;

    result = sqlite3_prepare_v2(
        db,
        stationSQL,
        -1,
        &stationStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to check station: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stationStatement, 1, stationID);

    result = sqlite3_step(stationStatement);

    if (result != SQLITE_ROW) {
        std::cout << "Charging station not found."
                  << std::endl;

        sqlite3_finalize(stationStatement);
        return false;
    }

    int availableSlots =
        sqlite3_column_int(stationStatement, 0);

    const char* status =
        reinterpret_cast<const char*>(
            sqlite3_column_text(stationStatement, 1)
        );

    std::string stationStatus =
        status ? status : "";

    sqlite3_finalize(stationStatement);

    if (stationStatus != "ACTIVE" &&
        stationStatus != "Active") {
        std::cout << "Charging station is not active."
                  << std::endl;
        return false;
    }

    if (availableSlots <= 0) {
        std::cout << "No charging slots available!"
                  << std::endl;
        return false;
    }

    // Create the charging request
    const char* insertSQL =
        "INSERT INTO ChargingRequests "
        "(UserID, VehicleID, StationID, Status) "
        "VALUES (?, ?, ?, 'PENDING');";

    sqlite3_stmt* insertStatement = nullptr;

    result = sqlite3_prepare_v2(
        db,
        insertSQL,
        -1,
        &insertStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to create request: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(insertStatement, 1, userID);
    sqlite3_bind_int(insertStatement, 2, vehicleID);
    sqlite3_bind_int(insertStatement, 3, stationID);

    result = sqlite3_step(insertStatement);

    sqlite3_finalize(insertStatement);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to create charging request: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Reduce available slots by 1
    const char* updateSQL =
        "UPDATE ChargingStations "
        "SET AvailableSlots = AvailableSlots - 1 "
        "WHERE StationID = ? AND AvailableSlots > 0;";

    sqlite3_stmt* updateStatement = nullptr;

    result = sqlite3_prepare_v2(
        db,
        updateSQL,
        -1,
        &updateStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to update charging station: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(updateStatement, 1, stationID);

    result = sqlite3_step(updateStatement);

    sqlite3_finalize(updateStatement);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to update charging slots: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::cout << "Charging request created successfully!"
              << std::endl;

    return true;
}

void Database::getChargingRequests() {

    const char* sql =
        "SELECT "
        "CR.RequestID, "
        "U.Name, "
        "V.VehicleNumber, "
        "CS.StationName, "
        "CR.RequestTime, "
        "CR.Status "
        "FROM ChargingRequests CR "
        "INNER JOIN Users U ON CR.UserID = U.UserID "
        "INNER JOIN Vehicles V ON CR.VehicleID = V.VehicleID "
        "INNER JOIN ChargingStations CS ON CR.StationID = CS.StationID;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to retrieve charging requests: "
                  << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "\n--- Charging Requests ---\n";

    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {

        int requestID =
            sqlite3_column_int(statement, 0);

        const unsigned char* userName =
            sqlite3_column_text(statement, 1);

        const unsigned char* vehicleNumber =
            sqlite3_column_text(statement, 2);

        const unsigned char* stationName =
            sqlite3_column_text(statement, 3);

        const unsigned char* requestTime =
            sqlite3_column_text(statement, 4);

        const unsigned char* status =
            sqlite3_column_text(statement, 5);

        std::cout << "Request ID: " << requestID << std::endl;
        std::cout << "User: " << userName << std::endl;
        std::cout << "Vehicle: " << vehicleNumber << std::endl;
        std::cout << "Station: " << stationName << std::endl;
        std::cout << "Request Time: " << requestTime << std::endl;
        std::cout << "Status: " << status << std::endl;
        std::cout << "-------------------------\n";
    }

    sqlite3_finalize(statement);
}

bool Database::updateRequestStatus(
    int requestID,
    const std::string& newStatus
) {

    const char* sql =
        "UPDATE ChargingRequests "
        "SET Status = ? "
        "WHERE RequestID = ?;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare status update: "
                  << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        newStatus.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(statement, 2, requestID);

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to update request status: "
                  << sqlite3_errmsg(db) << std::endl;

        sqlite3_finalize(statement);
        return false;
    }

    if (sqlite3_changes(db) == 0) {
        std::cout << "Request not found."
                  << std::endl;

        sqlite3_finalize(statement);
        return false;
    }

    sqlite3_finalize(statement);

    std::cout << "Request status updated successfully!"
              << std::endl;

    return true;
}

bool Database::completeChargingRequest(
    int requestID,
    double energyCharged
)
{        if (energyCharged <= 0)
    {
        std::cout << "Energy charged must be greater than 0."
                  << std::endl;
        return false;
    }

    const double pricePerKWh = 10.0;
    const double amount = energyCharged * pricePerKWh;
    // Start transaction
    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        db,
        "BEGIN TRANSACTION;",
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to start transaction: "
                  << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }

    // Find the station belonging to this request
    const char* findSQL =
        "SELECT StationID "
        "FROM ChargingRequests "
        "WHERE RequestID = ?;";

    sqlite3_stmt* findStatement = nullptr;

    result = sqlite3_prepare_v2(
        db,
        findSQL,
        -1,
        &findStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(findStatement, 1, requestID);

    result = sqlite3_step(findStatement);

    if (result != SQLITE_ROW) {
        std::cout << "Request not found." << std::endl;

        sqlite3_finalize(findStatement);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

        return false;
    }

    int stationID =
        sqlite3_column_int(findStatement, 0);

    sqlite3_finalize(findStatement);

    // Mark request as COMPLETED
    const char* updateRequestSQL =
        "UPDATE ChargingRequests "
        "SET Status = 'COMPLETED' "
        "WHERE RequestID = ?;";

    sqlite3_stmt* updateRequestStatement = nullptr;

    result = sqlite3_prepare_v2(
        db,
        updateRequestSQL,
        -1,
        &updateRequestStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(updateRequestStatement, 1, requestID);

    result = sqlite3_step(updateRequestStatement);

    sqlite3_finalize(updateRequestStatement);

    if (result != SQLITE_DONE) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    // Restore one available slot
    const char* updateStationSQL =
        "UPDATE ChargingStations "
        "SET AvailableSlots = AvailableSlots + 1 "
        "WHERE StationID = ? "
        "AND AvailableSlots < TotalSlots;";

    sqlite3_stmt* updateStationStatement = nullptr;

    result = sqlite3_prepare_v2(
        db,
        updateStationSQL,
        -1,
        &updateStationStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(updateStationStatement, 1, stationID);

    result = sqlite3_step(updateStationStatement);

    sqlite3_finalize(updateStationStatement);

    if (result != SQLITE_DONE || sqlite3_changes(db) == 0) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

        std::cerr << "Failed to restore station slot."
                  << std::endl;

        return false;
    }
         // Record the charging transaction
     const char* transactionSQL =
    "INSERT INTO Transactions "
    "(UserID, VehicleID, StationID, Amount, TransactionType, Status) "
    "SELECT UserID, VehicleID, StationID, ?, "
    "'CHARGING', 'COMPLETED' "
    "FROM ChargingRequests "
    "WHERE RequestID = ?;";

    sqlite3_stmt* transactionStatement = nullptr;

    result = sqlite3_prepare_v2(
        db,
        transactionSQL,
        -1,
        &transactionStatement,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        sqlite3_exec(
            db,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );

        std::cerr << "Failed to create transaction: "
                  << sqlite3_errmsg(db)
                  << std::endl;

        return false;
    }

    sqlite3_bind_double(
    transactionStatement,
    1,
    amount
);

sqlite3_bind_int(
    transactionStatement,
    2,
    requestID
);

    result = sqlite3_step(transactionStatement);

    sqlite3_finalize(transactionStatement);

    if (result != SQLITE_DONE)
    {
        sqlite3_exec(
            db,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );

        std::cerr << "Failed to record transaction: "
                  << sqlite3_errmsg(db)
                  << std::endl;

        return false;
    }

    // Everything succeeded
    result = sqlite3_exec(
        db,
        "COMMIT;",
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to commit transaction."
                  << std::endl;

        sqlite3_free(errorMessage);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

        return false;
    }

    std::cout << "Charging request completed successfully!"
              << std::endl;

    std::cout << "Charging slot restored."
              << std::endl;

    return true;
}

int Database::getLoggedInUserID(
    const std::string& email,
    const std::string& password
) {
    const char* sql =
        "SELECT UserID FROM Users "
        "WHERE Email = ? AND Password = ?;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Login query failed: "
                  << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(
        statement,
        1,
        email.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        2,
        password.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW) {
        int userID = sqlite3_column_int(statement, 0);

        sqlite3_finalize(statement);

        return userID;
    }

    sqlite3_finalize(statement);

    return -1;
}

void Database::getTransactions(int userID)
{
    const char* sql =
        "SELECT TransactionID, VehicleID, StationID, Amount, "
        "TransactionType, Status, CreatedAt "
        "FROM Transactions "
        "WHERE UserID = ? "
        "ORDER BY TransactionID DESC;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        std::cerr << "Failed to fetch transactions: "
                  << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(statement, 1, userID);

    std::cout << "\n--- My Transactions ---\n";

    bool found = false;

    while ((result = sqlite3_step(statement)) == SQLITE_ROW)
    {
        found = true;

        int transactionID =
            sqlite3_column_int(statement, 0);

        int vehicleID =
            sqlite3_column_int(statement, 1);

        int stationID =
            sqlite3_column_int(statement, 2);

        double amount =
            sqlite3_column_double(statement, 3);

        const unsigned char* transactionType =
            sqlite3_column_text(statement, 4);

        const unsigned char* status =
            sqlite3_column_text(statement, 5);

        const unsigned char* createdAt =
            sqlite3_column_text(statement, 6);

        std::cout << "Transaction ID: "
                  << transactionID << std::endl;

        std::cout << "Vehicle ID: "
                  << vehicleID << std::endl;

        std::cout << "Station ID: "
                  << stationID << std::endl;

        std::cout << "Amount: Rs."
                  << amount << std::endl;

        std::cout << "Transaction Type: "
                  << (transactionType
                      ? reinterpret_cast<const char*>(transactionType)
                      : "")
                  << std::endl;

        std::cout << "Status: "
                  << (status
                      ? reinterpret_cast<const char*>(status)
                      : "")
                  << std::endl;

        std::cout << "Created At: "
                  << (createdAt
                      ? reinterpret_cast<const char*>(createdAt)
                      : "")
                  << std::endl;

        std::cout << "-------------------------\n";
    }

    if (!found)
    {
        std::cout << "No transactions found."
                  << std::endl;
    }

    sqlite3_finalize(statement);
}