enum GEAR {
    N = 0,
    D1 = 1,
    D2 = 2,
    D3 = 3,
    D4 = 4,
    D5 = 5,
    D6 = 6,
    D7 = 7,
    D_CVT = 8,
    R_CVT = 9,
    R3 = 10,
    R = 11,
    R2 = 12,
    P = 13,
    NO_FORCE = 14,
    SNV = 15
};

enum FPC {
    Sport = 0,
    Comfort = 1,
    Not_defined = 2,
    SNV = 3
};

enum ErrorCheck {
    WAIT = 0, //  error check not yet run through completely
    Ok = 1, //  error check run through completely, result i. O
    ERROR = 2, // Error detected, enter current environment data
    NotDefined = 3
};

enum GearVariant {
    NAG_LARGE, // NAG - Large gear
    NAG_SMALL, // NAG - Small gear
    NAG2_GROSS2, // NAG2 - Large gear
    NAG2_KLEIN2 // NAG2 - Small gear
};

// Speed step
enum FSC {
    Blank = 32, // Blank gear
    Speed1 = 49, // Speed step 1
    Speed2 = 50, // Speed step 2
    Speed3 = 51, // Speed step 3
    Speed4 = 52, // Speed step 4
    Speed5 = 53, // Speed step 5
    Speed6 = 54, // Speed step 6
    Speed7 = 55, // Speed step 7
    A = 65, // Speed level 'A'
    D = 68, // Speed step 'D'
    F = 70, // Error mark 'F'
    N = 78, // Speed step 'N'
    P = 80, // Speed step 'P'
    R = 82, // Speed step 'R'
    Passive = 255, // Signal not avaliable
};

enum GearSelectorPos {
    P = 0, // Gear selector in park position 
    R = 1, // Gear selector in reverse position
    N = 2, // Gear selector in neutral position
    D = 4, // Gear selector in drive position
    Passive = 7, // Signal not avaliable
};

enum DrivingProgram {
    C_MCFB_WT = 2, // Message "GEAR, ACTUATE PARKING BRAKE!"", with warning tone
    C_MGSNN = 3, // Message "Shift gear! Shift lever to 'N!'"
    C_MGBB = 4, // Message "Gear, apply brake!"
    C_MGGEA = 6, // Message "Transmission, request gear again!"
    C_MGZSN = 7, // Message "Gear, insert to N to start!"

    A_MCFB_WT = 10, // Message "GEAR, ACTUATE PARKING BRAKE!"", with warning tone
    A_MGSNN = 11, // Message "Shift gear! Shift lever to 'N!'"
    A_MGBB = 12, // Message "Gear, apply brake!"
    A_MGGEA = 14, // Message "Transmission, request gear again!"
    A_MGZSN = 15, // Message "Gear, insert to N to start!"

    S_MCFB_WT = 18, // Message "GEAR, ACTUATE PARKING BRAKE!"", with warning tone
    S_MGSNN = 19, // Message "Shift gear! Shift lever to 'N!'"
    S_MGBB = 20, // Message "Gear, apply brake!"
    S_MGGEA = 22, // Message "Transmission, request gear again!"
    S_MGZSN = 23, // Message "Gear, insert to N to start!"

    UP = 24, // Shift recommendation "Upshift"
    DOWN = 25, // Shift recommendation "Downshift"

    BLANK = 32, // Blank
    BLANK_MGN = 64, // Blank - Message: "Shift gear! Shift level to 'N'!"

    A = 65, // Driving mode Agility (A)
    C = 67, // Driving mode Comfort (C)
    F = 70, // F error mark! (F)
    M = 77, // Driving mode Manual (M)
    S = 83, // Driving mode Sport (S)
    W = 87, // Driving mode Winter (W)
    U = 95, // Driving mode underscore (_)

    BLANK_MGW = 96, // Blank - Message: "Gearbox, visit workshop!"

    A_MGN = 97, // Message: "Gear, insert to N!" - Agility mode
    C_MGN = 99, // Message: "Gear, insert to N!" - Comfort mode
    M_MGN = 109, // Message: "Gear, insert to N!" - Manual mode
    S_MGN = 115, // Message: "Gear, insert to N!" - Sport mode
    W_MGN = 119, // Message: "Gear, insert to N!" - Winter mode
    U_MGN = 127, // Message: "Gear, insert to N!" - No mode (_)
    
    A_MGW = 129, // Message "Gearbox! Visit workshop!" - Agility mode
    C_MGW = 131, // Message "Gearbox! Visit workshop!" - Comfort mode
    F_MGW = 134, // Message "Gearbox! Visit workshop!" - Fail mode (F)
    M_MGW = 141, // Message "Gearbox! Visit workshop!" - Manual mode
    S_MGW = 147, // Message "Gearbox! Visit workshop!" - Sport mode
    W_MGW = 151, // Message "Gearbox! Visit workshop!" - Winter mode
    U_MGW = 159, // Message "Gearbox! Visit workshop!" - No mode (_)

    PASSIVE = 255, // Passive value (Signal not avaliable)
};