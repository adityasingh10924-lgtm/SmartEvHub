PRAGMA foreign_keys = ON;

CREATE TABLE Users (
    UserID INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT NOT NULL,
    Email TEXT UNIQUE NOT NULL,
    Password TEXT NOT NULL
);

CREATE TABLE Vehicles (
    VehicleID INTEGER PRIMARY KEY AUTOINCREMENT,
    UserID INTEGER NOT NULL,
    VehicleNumber TEXT UNIQUE NOT NULL,
    VehicleType TEXT,
    BatteryCapacity REAL,
    FOREIGN KEY (UserID) REFERENCES Users(UserID)
);

CREATE TABLE ChargingStations (
    StationID INTEGER PRIMARY KEY AUTOINCREMENT,
    StationName TEXT NOT NULL,
    Location TEXT NOT NULL,
    TotalSlots INTEGER NOT NULL,
    AvailableSlots INTEGER NOT NULL,
    Status TEXT NOT NULL
);

CREATE TABLE ChargingRequests (
    RequestID INTEGER PRIMARY KEY AUTOINCREMENT,
    UserID INTEGER NOT NULL,
    VehicleID INTEGER NOT NULL,
    StationID INTEGER NOT NULL,
    RequestTime TEXT DEFAULT CURRENT_TIMESTAMP,
    Status TEXT NOT NULL,
    FOREIGN KEY (UserID) REFERENCES Users(UserID),
    FOREIGN KEY (VehicleID) REFERENCES Vehicles(VehicleID),
    FOREIGN KEY (StationID) REFERENCES ChargingStations(StationID)
);

CREATE TABLE Transactions (
    TransactionID INTEGER PRIMARY KEY AUTOINCREMENT,
    UserID INTEGER NOT NULL,
    VehicleID INTEGER,
    StationID INTEGER,
    Amount REAL,
    TransactionType TEXT,
    Status TEXT,
    CreatedAt TEXT DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (UserID) REFERENCES Users(UserID),
    FOREIGN KEY (VehicleID) REFERENCES Vehicles(VehicleID),
    FOREIGN KEY (StationID) REFERENCES ChargingStations(StationID)
);

CREATE TABLE ChargingSessions (
    SessionID INTEGER PRIMARY KEY AUTOINCREMENT,
    RequestID INTEGER NOT NULL,
    UserID INTEGER NOT NULL,
    VehicleID INTEGER NOT NULL,
    StationID INTEGER NOT NULL,
    StartTime TEXT DEFAULT CURRENT_TIMESTAMP,
    EndTime TEXT,
    EnergyCharged REAL DEFAULT 0,
    Status TEXT NOT NULL,
    FOREIGN KEY (RequestID) REFERENCES ChargingRequests(RequestID),
    FOREIGN KEY (UserID) REFERENCES Users(UserID),
    FOREIGN KEY (VehicleID) REFERENCES Vehicles(VehicleID),
    FOREIGN KEY (StationID) REFERENCES ChargingStations(StationID)
);
