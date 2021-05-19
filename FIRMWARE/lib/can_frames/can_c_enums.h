#ifndef CAN_C_ENUMS_H_
#define CAN_C_ENUMS_H_

// -- ESP/BAS Enums -- //

enum class BS_BLS {
    BREMSE_NBET = 0,
    BREMSE_BET = 1,
    NA = 2,
    SNV = 255
};

enum class BS_DRT {
    PASSIVE = 0,
    FORWARD = 1,
    REVERSE = 2,
    SNV = 3
};

enum class GMIN {
    PASSIVE = 0,
    G1 = 1,
    G2 = 2,
    G3 = 3,
    G4 = 4,
    G5 = 5,
    G6 = 6,
    G7 = 7
};

enum class GMAX {
    PASSIVE = 0,
    G1 = 1,
    G2 = 2,
    G3 = 3,
    G4 = 4,
    G5 = 5,
    G6 = 6,
    G7 = 7
};

enum BS_SLV {
    SKL0 = 0,
    SKL1 = 1,
    SKL2 = 2,
    SKL3 = 3,
    SKL4 = 4,
    SKL5 = 5,
    SKL6 = 6,
    SKL7 = 7,
    SKL8 = 8,
    SKL9 = 9,
    SKL10 = 10
};

enum class BS_SZS {
    ERR_DIAG = 0,
    NORM = 1,
    NA = 2,
    EXHAUST = 3
};

enum class BS_ANFN {
    NA = 0,
    ANF_N = 1,
    IDLE = 2,
    SNV = 3
};

enum class BS_PRW_ST {
    ON = 0,
    WARN = 1,
    OFF = 2,
    INIT = 3,
    NA1 = 4,
    NA2 = 5,
    PRW_NV = 6,
    SNV = 7
};

enum class BS_PRW_WARN {
    OK = 0,
    WARN_ONCE = 1,
    PRW_NV = 2,
    PRW_START = 3,
    WARN_VL = 4,
    WARN_VR = 5,
    WARN_HL = 6,
    WARN_HR = 7,
    NA = 8,
    UNDEF = 14,
    SNV = 15
};

enum class BS_SFB {
    BRAKE_NO = 0,
    BREMSE_YES = 1,
    NA = 2,
    SNV = 3
};

enum class BS_T_Z {
    NA = 0,
    T20_0 = 1,
    T23_1 = 2,
    SNV = 3
};



// -- GEARBOX ENUMS -- //
enum class GS_GIC {
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

enum class GS_GZC {
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
    CANCEL = 14,
    SNV = 15
};

enum class GS_FPC_AAD {
    SPORT = 0,
    COMFORT = 1,
    NA = 2,
    SNV = 3
};

enum class GS_ST {
    WAIT = 0, //  error check not yet run through completely
    Ok = 1, //  error check run through completely, result i. O
    ERROR = 2, // Error detected, enter current environment data
    NA = 3
};

enum class GS_FSC {
    BLANK = 32, // Blank gear
    ONE = 49, // Speed step 1
    TWO = 50, // Speed step 2
    THREE = 51, // Speed step 3
    FOUR = 52, // Speed step 4
    FIVE = 53, // Speed step 5
    SIX = 54, // Speed step 6
    SEVEN = 55, // Speed step 7
    A = 65, // Speed level 'A'
    D = 68, // Speed step 'D'
    F = 70, // Error mark 'F'
    N = 78, // Speed step 'N'
    P = 80, // Speed step 'P'
    R = 82, // Speed step 'R'
    SNV = 255, // Signal not avaliable
};

enum class GS_MECH {
    NAG_LARGE = 0, // NAG - Large gear
    NAG_SMALL = 1, // NAG - Small gear
    NAG2_GROSS2 = 2, // NAG2 - Large gear
    NAG2_KLEIN2 = 3 // NAG2 - Small gear
};

enum GS_WHST {
    P = 0, // Gear selector in park position 
    R = 1, // Gear selector in reverse position
    N = 2, // Gear selector in neutral position
    D = 4, // Gear selector in drive position
    SNV = 7, // Signal not avaliable
};

enum class GS_FPC {
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

// -- EWM Enums -- //
enum class EWM_WHC {
    D = 5,
    N = 6,
    R = 7,
    P = 8,
    PLUS = 9,
    MINUS = 10,
    N_D = 11,
    R_N = 12,
    P_R = 13,
    SNV = 15
};

#endif
