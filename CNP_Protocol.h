/******************************************/
/**
  @brief Contains type definitions required to support 5580 Computer Networks 
         Project (CNP) Protocol 
  @main
  @file   CNP_Protocol.h
  @author
  @date   March 25, 2015

  <b> Objective: </b>

    Objective of this group activity is to design an application layer protocol 
    standard that will be used by all groups for the project
 
  <b> Implementation Notes </b>

    1.  Those types with the prefixed '_' are intentionally 'uglified' to discourage their direct use.
        Additionally, they have further been wrapped in the 'prim' namespace to further obscure them
        from direct use.

        This is a common naming procedure used to denote an 'internal' type within a published
        API specification or protocol to not directly use.

    2.  Regarding inheritance.

        Even though, from an implementation stand-point, C++ inheritance would have afforded
        the ability to avoid a lot of "helper" method duplication.  However, from the position of
        affording any byte-wise guarantee of consistency across various compilers and platforms,
        there was no way to insure that the use of inheritance would not, covertly, introduce
        additional hidden bytes or other compiler generated information.

        So, it was explicitly avoided in the message protocol implementation.

 ******************************************/

#ifndef _CNP_PROTOCOL_H__
#define _CNP_PROTOCOL_H__

#include <string.h>

// set structure alignment to 1 byte
#pragma pack(push, 1)

namespace cnp
{
    typedef unsigned short      WORD;   ///< 16bit type definition
    typedef unsigned long       DWORD;  ///< 32bit type definition
    typedef unsigned long long  QWORD;  ///< 64bit type definition

/// Helper macro that calculates count of elements in an array
#ifndef COUNTOF
    #define COUNTOF(_array)  (sizeof(_array) / sizeof(_array[0]))
#endif

#ifndef MAKE_CNP_MSG_TYPE
    #define   MAKE_CNP_MSG_TYPE(type, sub) ((sub << 16) + type)
#endif

#ifndef MAKE_CNP_ERROR_RESULT
    #define   MAKE_CNP_ERROR_RESULT(facility, sub) ((facility << 16) + sub)
#endif

/**
   @brief Global message sequence number

   The following is a little complicated, but I will try to explain what is going on.

   g_dwSequenceNumber is a static global variable intended to be auto-incremented by
   the client as part of constructing request messages.  It needs to be defined such 
   that it is accessible by various message constructors, but at the same time does 
   not create a linker error as it can be included in multiple .cpp files. 
   (resulting in multiple instances across various COMDATs)

   So, we are telling the linker to just use one of the instances here if it is
   finds multiple instance declarations.

   Another complication is that MSVC++ & GNUC++ cannot agree on how to do this.

   @sa https://gcc.gnu.org/onlinedocs/gcc/Vague-Linkage.html
   @sa https://msdn.microsoft.com/EN-US/library/5tkz6s71(v=VS.120,d=hv.2).aspx
*/
#ifdef  __GNUC__
    __attribute__((weak)) DWORD g_dwSequenceNumber = 0;
#elif   _MSC_VER
    __declspec(selectany) extern DWORD g_dwSequenceNumber = 0;
#else
    #pragma message("Compiler not support")
#endif

// ============== const definitions =====================

/// CNP Protocol version
 const WORD  g_wMajorVersion   = 1;  ///< Protocol major version (i.e. 1.x)
 const WORD  g_wMinorVersion   = 1;  ///< Protocol minor version (i.e. x.3)

 /// CNP Validation Key
 const DWORD g_dwValidationKey = 0x00DEAD01;

/// [first,last] name field lengths
/**
    @sa CNP_CREATE_ACCOUNT_REQUEST, CNP_LOGON_REQUEST
*/
 const size_t MAX_NAME_LEN     = 32;

 /// Used for error checking & default initialization
 const WORD   INVALID_CLIENT_ID = (~0);

/// Supported CNP Message Types (CMT_)
enum CNP_MSG_TYPE
{
    CMT_INVALID           = 0x00,  ///< used for initialization and error checking
    CMT_CONNECT           = 0x50,
    CMT_CREATE_ACCOUNT    = 0x51,
    CMT_LOGON             = 0x52,
    CMT_LOGOFF            = 0x53,
    CMT_DEPOSIT           = 0x54,
    CMT_WITHDRAWAL        = 0x55,
    CMT_BALANCE_QUERY     = 0x56,
    CMT_TRANSACTION_QUERY = 0x57,
    CMT_PURCHASE_STAMPS   = 0x58
};

/// Supported CNP Message Subtypes (CMS_)
enum CNP_MSG_SUBTYPE
{
    CMS_INVALID           = 0x00,  ///< used for initialization and error checking
    CMS_REQUEST           = 0x01,
    CMS_RESPONSE          = 0x02
};

/// Constructed Message Type IDs
enum MSG_TYPE
{
     MT_INVALID_ID                       = MAKE_CNP_MSG_TYPE(CMT_INVALID, CMS_INVALID),

     MT_CONNECT_REQUEST_ID               = MAKE_CNP_MSG_TYPE(CMT_CONNECT, CMS_REQUEST),
     MT_CONNECT_RESPONSE_ID              = MAKE_CNP_MSG_TYPE(CMT_CONNECT, CMS_RESPONSE),

     MT_CREATE_ACCOUNT_REQUEST_ID        = MAKE_CNP_MSG_TYPE(CMT_CREATE_ACCOUNT, CMS_REQUEST),
     MT_CREATE_ACCOUNT_RESPONSE_ID       = MAKE_CNP_MSG_TYPE(CMT_CREATE_ACCOUNT, CMS_RESPONSE),

     MT_LOGON_REQUEST_ID                 = MAKE_CNP_MSG_TYPE(CMT_LOGON, CMS_REQUEST),
     MT_LOGON_RESPONSE_ID                = MAKE_CNP_MSG_TYPE(CMT_LOGON, CMS_RESPONSE),

     MT_LOGOFF_REQUEST_ID                = MAKE_CNP_MSG_TYPE(CMT_LOGOFF, CMS_REQUEST),
     MT_LOGOFF_RESPONSE_ID               = MAKE_CNP_MSG_TYPE(CMT_LOGOFF, CMS_RESPONSE),

     MT_DEPOSIT_REQUEST_ID               = MAKE_CNP_MSG_TYPE(CMT_DEPOSIT, CMS_REQUEST),
     MT_DEPOSIT_RESPONSE_ID              = MAKE_CNP_MSG_TYPE(CMT_DEPOSIT, CMS_RESPONSE),

     MT_WITHDRAWAL_REQUEST_ID            = MAKE_CNP_MSG_TYPE(CMT_WITHDRAWAL, CMS_REQUEST),
     MT_WITHDRAWAL_RESPONSE_ID           = MAKE_CNP_MSG_TYPE(CMT_WITHDRAWAL, CMS_RESPONSE),

     MT_BALANCE_QUERY_REQUEST_ID         = MAKE_CNP_MSG_TYPE(CMT_BALANCE_QUERY, CMS_REQUEST),
     MT_BALANCE_QUERY_RESPONSE_ID        = MAKE_CNP_MSG_TYPE(CMT_BALANCE_QUERY, CMS_RESPONSE),

     MT_TRANSACTION_QUERY_REQUEST_ID     = MAKE_CNP_MSG_TYPE(CMT_TRANSACTION_QUERY, CMS_REQUEST),
     MT_TRANSACTION_QUERY_RESPONSE_ID    = MAKE_CNP_MSG_TYPE(CMT_TRANSACTION_QUERY, CMS_RESPONSE),

     MT_PURCHASE_STAMPS_REQUEST_ID       = MAKE_CNP_MSG_TYPE(CMT_PURCHASE_STAMPS, CMS_REQUEST),
     MT_PURCHASE_STAMPS_RESPONSE_ID      = MAKE_CNP_MSG_TYPE(CMT_PURCHASE_STAMPS, CMS_RESPONSE)
};
/**
    @brief MT Facility Code Types (CFC)

    This enum is used in the creation of result codes returned to the client.
    The purpose is to help provide the client useful diagnostic information
    regarding associating specific errors with a particular facility or
    functional subsystem.
*/
enum CFC_TYPE
{
    CFC_CONNECT,             ///< Connection validation related issues 
    CFC_CREDENTIALS,         ///< Logon related issues
    CFC_FUNCTIONAL,          ///< Invalid arguments or Client state
    CFC_ACCOUNT,             ///< Account related errors related to balances, etc.
    CFC_UNDEFINED            ///< Other error categories not explicitly defined
};

/** 
    @brief CNP Error Result Types (CER)
*/
enum CER_TYPE
{
    CER_SUCCESS              = 0,  ///< Success!
    CER_AUTHENICATION_FAILED = MAKE_CNP_ERROR_RESULT(CFC_CONNECT, 0x01),     ///< Invalid validation key
    CER_UNSUPPORTED_PROTOCOL = MAKE_CNP_ERROR_RESULT(CFC_CONNECT, 0x02),     ///< Protocol version not supported
    CER_INVALID_CLIENTID     = MAKE_CNP_ERROR_RESULT(CFC_CREDENTIALS, 0x01), ///< Invalid client ID found
    CER_INVALID_NAME_PIN     = MAKE_CNP_ERROR_RESULT(CFC_CREDENTIALS, 0x02), ///< Invalid name or pin
    CER_INVALID_ARGUMENTS    = MAKE_CNP_ERROR_RESULT(CFC_FUNCTIONAL, 0x01),  ///< Invalid arguments used
    CER_CLIENT_NOT_LOGGEDON  = MAKE_CNP_ERROR_RESULT(CFC_FUNCTIONAL, 0x02),  ///< Client not logged-on
    CER_DRAWER_BLOCKED       = MAKE_CNP_ERROR_RESULT(CFC_FUNCTIONAL, 0x03),  ///< Mechanical Failure
    CER_INSUFFICIENT_FUNDS   = MAKE_CNP_ERROR_RESULT(CFC_ACCOUNT, 0x01),     ///< Insufficient funds available

    CER_ERROR                = (~0)     ///< Generic error result
};


/// CNP Deposit types (DT)
enum DEPOSIT_TYPE
{
    DT_INVALID   = 0,       ///< for initialization and error checking
    DT_CASH      = 0x01,
    DT_CHECK     = 0x02
};

/// CNP Transaction types (TT)
enum TRANSACTION_TYPE
{
    TT_INVALID    = 0,     ///< for initialization and error checking
    TT_DEPOSIT    = 0x01,
    TT_WITHDRAWAL = 0x02
};

/**
    @sa TRANSACTION_TYPE
*/

/// A transaction record
struct TRANSACTION
{
    DWORD                      m_dwID;        ///< A Server generated unique sequential ID associated with each transaction
    QWORD                      m_qwDateTime;  ///< a 64bit UTC value that represents number of seconds since Epoch
    DWORD                      m_dwAmount;    ///< Amount excluding decimal point (i.e. $100.00 would be 10000)
    WORD                       m_wType;       ///< The transaction type, represented as TT_DEPOSIT or TT_WITHDRAWAL
};

// =============================== CNP Message Primitives =======================================
namespace prim
{
/// Connection Request Primitive
struct _CONNECTION_REQUEST
{
    WORD                       m_wMajorVersion;    ///< Client Major Protocol version number
    WORD                       m_wMinorVersion;    ///< Client Minor Protocol version number
    DWORD                      m_dwValidationKey;  ///< Used by server to authenticate the connection

    /// Default constructor
    _CONNECTION_REQUEST()
        : m_wMajorVersion(0),
          m_wMinorVersion(0),
          m_dwValidationKey(0)
    { };
    /// Initialization constructor
    _CONNECTION_REQUEST(WORD  wMajorVersion,
                        WORD  wMinorVersion, 
                        DWORD dwKey)
        : m_wMajorVersion  (wMajorVersion),
          m_wMinorVersion  (wMinorVersion),
          m_dwValidationKey(dwKey)
    { };
};

/// Connection Response Result Primitive
/**
    @sa CER_TYPE
*/
struct _CONNECTION_RESPONSE
{
     DWORD                     m_dwResult;         ///< Contain Success or Error code from CER_TYPE
     WORD                      m_wMajorVersion;    ///< Server Major Protocol version number
     WORD                      m_wMinorVersion;    ///< Server Minor Protocol version number
     WORD                      m_wClientID;        ///< this field is generated by the Server and is required in all subsequent request messages by the client

    _CONNECTION_RESPONSE(DWORD dwResult = cnp::CER_ERROR,        ///< Server generated result
                         WORD  wMajorVersion = 0, 
                         WORD  wMinorVersion = 0, 
                         WORD  wClientID = INVALID_CLIENT_ID)    ///< Server generated ClientID
        : m_dwResult(dwResult),
          m_wMajorVersion(wMajorVersion),
          m_wMinorVersion(wMinorVersion),
          m_wClientID(wClientID)
    { };
};

/// Create Account Request Primitive
struct _CREATE_ACCOUNT_REQUEST
{
    char                       m_szFirstName[MAX_NAME_LEN]; ///< User's First Name
    char                       m_szLastName[MAX_NAME_LEN];  ///< User's Last Name
    char                       m_szEmailAddress[MAX_NAME_LEN]; 
    WORD                       m_wPIN;                      ///< User's Personal Identification Number
    DWORD                      m_dwSSNumber;                ///< (optional) User's Social Security Number
    DWORD                      m_dwDLNumber;                ///< (optional) User's Drivers License Number

    /// Default constructor
    _CREATE_ACCOUNT_REQUEST()
        : m_wPIN(0),
          m_dwSSNumber(0),
          m_dwDLNumber(0)
    { memset(m_szFirstName, 0, sizeof(m_szFirstName) + sizeof(m_szLastName) + sizeof (m_szEmailAddress) ); };

    /// Initialization constructor
    _CREATE_ACCOUNT_REQUEST(const char* szFirstName, 
                            const char* szLastName,
                            const char* szEmailAddress,
                            WORD wPIN, DWORD dwSSNumber, DWORD dwDLNumber)
        : m_wPIN(wPIN),
          m_dwSSNumber(dwSSNumber),
          m_dwDLNumber(dwDLNumber)
    { memset(m_szFirstName, 0, sizeof(m_szFirstName) + sizeof(m_szLastName) + sizeof (m_szEmailAddress) );
      set_FirstName(szFirstName);
      set_LastName(szLastName); 
      set_EmailAddress(szEmailAddress);
    };

/**
    @param [in] szSet
*/
    void set_FirstName(const char* szSet)
    { if (szSet)
         strncpy(m_szFirstName, szSet, COUNTOF(m_szFirstName) - 1); };

/**
    @param [in] szSet
*/
    void set_LastName(const char* szSet)
    { if (szSet)
         strncpy(m_szLastName, szSet, COUNTOF(m_szLastName) - 1); };

/**
    @param [in] szSet
*/
    void set_EmailAddress(const char* szSet)
    { if (szSet)
      strncpy(m_szEmailAddress, szSet, COUNTOF(m_szEmailAddress) - 1); };
};

/// Create Account Response Result Primitive
struct _CREATE_ACCOUNT_RESPONSE
{
    DWORD                      m_dwResult;                 ///< Contain Success or Error code from CER_TYPE
    QWORD                      m_qwPAN;                    ///< Primary Account NO.

    _CREATE_ACCOUNT_RESPONSE(DWORD dwResult = cnp::CER_ERROR)
        : m_dwResult(dwResult)
    { };
};

/// Logon Request Primitive
struct _LOGON_REQUEST
{
    QWORD                      m_PAN;//< account number
    //char                       m_szFirstName[MAX_NAME_LEN]; ///< User's first name
    WORD                       m_wPIN;                      ///< Personal Identification Number

    /// Initialization constructor
    _LOGON_REQUEST(QWORD qwPAN = 0, WORD wPIN = 0)
        : m_wPIN(wPIN), m_PAN(qwPAN)
    {};

};

/// Logon Response Result Primitive
/**
    @sa CER_TYPE
*/
struct _LOGON_RESPONSE
{
    DWORD                      m_dwResult;   ///< Contain Success or Error code from CER_TYPE
    
    _LOGON_RESPONSE(DWORD dwResult = cnp::CER_ERROR)
        : m_dwResult(dwResult)
    { };
};

/// Logoff Request Primitive
struct _LOGOFF_REQUEST
{
    _LOGOFF_REQUEST()
    { };
};

/// Logoff Response Result Primitive
/**
    @sa CER_TYPE
*/
struct _LOGOFF_RESPONSE
{
    DWORD                      m_dwResult;   ///< Contain Success or Error code from CER_TYPE

    _LOGOFF_RESPONSE(DWORD dwResult = cnp::CER_ERROR)
        : m_dwResult(dwResult)
    { };
};

struct _ERROR_RESPONSE
{
    DWORD                   m_dwResult;
    _ERROR_RESPONSE(DWORD dwResult = cnp::CER_ERROR)
    : m_dwResult(dwResult)
            { };
};



/// Deposit Request Primitive
/**
    @sa DEPOSIT_TYPE
*/
struct _DEPOSIT_REQUEST
{
    DWORD                      m_dwAmount;  ///< Amount excluding decimal point (i.e. $100.00 would be 10000)
    WORD                       m_wType;     ///< DT_CASH or DT_CHECK

    /// Initialization constructor
    _DEPOSIT_REQUEST(DWORD dwAmount = 0, DEPOSIT_TYPE Type = DT_INVALID)
        : m_dwAmount(dwAmount),
          m_wType(Type)
    { };
};

/// Deposit Response Result Primitive
/**
    @sa CER_TYPE
*/
struct _DEPOSIT_RESPONSE
{
    DWORD                      m_dwResult;    ///< Contain Success or Error code from CER_TYPE

    _DEPOSIT_RESPONSE(DWORD dwResult = cnp::CER_ERROR)
        : m_dwResult(dwResult)
    { };
};

/// Withdrawal Request Primitive
struct _WITHDRAWAL_REQUEST
{
    DWORD                      m_dwAmount;    ///< Amount excluding decimal point (i.e. $100.00 would be 10000)

    /// Default constructor
    _WITHDRAWAL_REQUEST()
        : m_dwAmount(0)
    { };

    /// Initialization constructor
    _WITHDRAWAL_REQUEST(DWORD dwAmount)
        : m_dwAmount(dwAmount)
    { };
};

/// Withdrawal Response Result Primitive
/**
    @sa CER_TYPE
*/
struct _WITHDRAWAL_RESPONSE
{
    DWORD                      m_dwResult;    ///< Contain Success or Error code from CER_TYPE

    _WITHDRAWAL_RESPONSE(DWORD dwResult = cnp::CER_ERROR)
        : m_dwResult(dwResult)
    { };
};

/// Purchase Stamp Request Primitive
struct _STAMP_PURCHASE_REQUEST
{
    DWORD                      m_dwAmount;    ///< Amount excluding decimal point (i.e. $100.00 would be 10000)

    /// Default constructor
    _STAMP_PURCHASE_REQUEST()
        : m_dwAmount(0)
    { };

    /// Initialization constructor
    _STAMP_PURCHASE_REQUEST(DWORD dwAmount)
        : m_dwAmount(dwAmount)
    { };
};

/// Purchase Stamp Result Primitive
/**
    @sa CER_TYPE
*/
struct _STAMP_PURCHASE_RESPONSE
{
    DWORD                      m_dwResult;    ///< Contain Success or Error code from CER_TYPE

    _STAMP_PURCHASE_RESPONSE(DWORD dwResult = cnp::CER_ERROR)
        : m_dwResult(dwResult)
    { };
};

/// Transaction Query Request Primitive
struct _TRANSACTION_QUERY_REQUEST
{
    DWORD                      m_dwStartID;           ///< the transaction number to begin the query from
    WORD                       m_wTransactionCount;   ///< the number of transactions requested

    /// Default constructor
    _TRANSACTION_QUERY_REQUEST()
        : m_dwStartID(0),
          m_wTransactionCount(0)
    { };

    /// Initialization constructor
    _TRANSACTION_QUERY_REQUEST(DWORD dwStartID, WORD wTransactionCount)
        : m_dwStartID(dwStartID),
          m_wTransactionCount(wTransactionCount)
    { };
};


/// Transaction Query Result Primitive
/**
    @sa CER_TYPE
*/
struct _TRANSACTION_QUERY_RESPONSE
{
    DWORD                      m_dwResult;          ///< Contain Success or Error code from CER_TYPE
    WORD                       m_wTransactionCount; ///< number of transactions returned in array

    /*struct hack, dynamically grow this field to any number of transaction entities*/
    TRANSACTION                m_rgTransactions[1];  ///< array of unspecified size of Transaction records

    _TRANSACTION_QUERY_RESPONSE(DWORD dwResult = cnp::CER_ERROR, WORD wTransactionCount = 0)
        : m_dwResult(dwResult),
          m_wTransactionCount(wTransactionCount)
    { };
};

/// Balance Query Request Primitive
struct _BALANCE_QUERY_REQUEST
{
    _BALANCE_QUERY_REQUEST()
    { };
};

/// Balance Query Result Primitive
/**
    @sa CER_TYPE
*/
struct _BALANCE_QUERY_RESPONSE
{
    DWORD                      m_dwResult;    ///< Contain Success or Error code from CER_TYPE
    DWORD                      m_dwCashBalance;   ///< Client account balance
    DWORD                      m_dwCheckBalance;

    _BALANCE_QUERY_RESPONSE(DWORD dwResult = cnp::CER_ERROR, DWORD dwCashBalance = 0, DWORD wCheckBalance = 0)
        : m_dwResult(dwResult), m_dwCashBalance(dwCashBalance), m_dwCheckBalance(wCheckBalance)
    { };
};

}

/// CNP Standard Message Header
/**
   This is the header message that is provided as part of all CNP Request and Response 
   messages.  The m_wDataLen field is subsequently calculated as:

        sizeof(m_Request) or sizeof(m_Response)
 
   to get the size of message excluding the size of the header.

   The m_dwContext field is exclusively reserved for application use.  The contents 
   of this field is returned to the user in the corresponding result message structure 
   without modification.

   The m_wClientID value is return by the server in the CONNECTION_RESPONSE message
   and is required in all subsequent messages sent by the client to the server.

   @sa CONNECTION_RESPONSE
*/
struct STD_HDR
{
    DWORD   m_dwMsgType;  ///< Message Type
    WORD    m_wDataLen;   ///< Message data length excluding this header
    WORD    m_wClientID;  ///< Client ID, set by the CNP Server & used required by Client in subsequent messages
    DWORD   m_dwSequence; ///< Incremented by the Client, used for the client to match Server responses to client requests
    DWORD   m_dwContext;  ///< Optional field, reserved for the Client

    /// Default constructor
    STD_HDR()
        : m_dwMsgType (MT_INVALID_ID),
          m_wDataLen  (0),
          m_wClientID (INVALID_CLIENT_ID),
          m_dwSequence(0),
          m_dwContext (0)
    { };

    /// Initialization constructor
    STD_HDR(DWORD dwMsgType, 
            WORD  wDataLen, 
            WORD  wClientID  = INVALID_CLIENT_ID, 
            DWORD dwSequence = 0,  ///< incremented by the Client
            DWORD dwContext  = 0)  ///< optional field for the Client
        : m_dwMsgType (dwMsgType),
          m_wDataLen  (wDataLen),
          m_wClientID (wClientID),
          m_dwSequence(dwSequence),
          m_dwContext (dwContext)
    { };

    DWORD get_MsgType(void) const
    { return m_dwMsgType; };

    WORD  get_ClientID(void) const
    { return m_wClientID; };

    DWORD get_Sequence(void) const
    { return m_dwSequence; };

    DWORD get_Context(void) const
    { return m_dwContext; };
};

/**
  @brief The connection request message establishes an authenticated connection 
         with the server.

         No Client ID is required for this message.
*/
/// [Client] Connect Request message
struct CONNECT_REQUEST
{
    STD_HDR                    m_Hdr;
    prim::_CONNECTION_REQUEST  m_Request;

/// Initialization constructor
/**
  Auto increments the global sequence number
*/
    CONNECT_REQUEST(WORD  wClientID       = 0, ///< This value is ignored by server on initial connect 
                    WORD  wMajorVersion   = g_wMajorVersion, 
                    WORD  wMinorVersion   = g_wMinorVersion, 
                    DWORD dwValidationKey = g_dwValidationKey,
                    DWORD dwContext       = 0) ///< Optional field for the client
        : m_Hdr( MT_CONNECT_REQUEST_ID, sizeof(m_Request), wClientID, g_dwSequenceNumber++, dwContext ),
          m_Request(wMajorVersion, wMinorVersion, dwValidationKey)
    { };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t get_Size(void) const
    { return sizeof(*this); };

/// ===================================================
/// Server Decoding Helper Methods

    DWORD      get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD       get_ClientID(void) const
    { return m_Hdr.get_ClientID(); };

    DWORD      get_Sequence(void) const
    { return m_Hdr.get_Sequence(); };

    DWORD       get_Context(void) const
    { return m_Hdr.get_Context(); };

    WORD        get_ClientMajorVersion(void) const
    { return m_Request.m_wMajorVersion; };

    WORD        get_ClientMinorVersion(void) const
    { return m_Request.m_wMinorVersion; };

    DWORD       get_ClientValidationKey(void) const
    { return m_Request.m_dwValidationKey; };
 
};

/// [Server] Connection Response message
struct CONNECT_RESPONSE
{
    STD_HDR                     m_Hdr;
    prim::_CONNECTION_RESPONSE  m_Response;

/// Default Constructor
    CONNECT_RESPONSE()
        : m_Hdr(),
          m_Response()
    { };

/// Initialization Constructor
    CONNECT_RESPONSE(DWORD dwResult,         ///< Server generated result
                     WORD  wClientID,        ///< Server generated ClientID (this msg only)
                     WORD  wMajorVersion,    ///< current server version 
                     WORD  wMinorVersion,    ///< current server version
                     DWORD dwSequenceNumber, ///< copied from CNP_CONNECT_REQUEST
                     DWORD dwContext)        ///< copied from CNP_CONNECT_REQUEST
        : m_Hdr(MT_CONNECT_RESPONSE_ID, sizeof(m_Response), wClientID, dwSequenceNumber, dwContext),
          m_Response(dwResult, wMajorVersion, wMinorVersion, wClientID)
    { };

    DWORD    get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD     get_ClientID(void) const
    { return m_Response.m_wClientID; };

    DWORD    get_ResponseResult(void) const
    { return m_Response.m_dwResult; };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t    get_Size(void) const
    { return sizeof(*this); };
};

/// [Client] Create Account Request message
struct CREATE_ACCOUNT_REQUEST
{
    STD_HDR                        m_Hdr;
    prim::_CREATE_ACCOUNT_REQUEST  m_Request;

/// Default constructor
    CREATE_ACCOUNT_REQUEST()
        : m_Hdr(MT_CREATE_ACCOUNT_REQUEST_ID, sizeof(m_Request), 0, g_dwSequenceNumber++, 0),
          m_Request()
    { };

 /// Initialization constructor
    CREATE_ACCOUNT_REQUEST(WORD wClientID, 
                           const char* szFirstName, 
                           const char* szLastName,
                           const char* szEmailAddress,
                           WORD  wPin,       ///< Client Generated PIN
                           DWORD dwSSN = 0,  ///< optional field 
                           DWORD dwDLN = 0,  ///< optional field
                           DWORD dwClientContext = 0)  ///< optional field for the Client
        : m_Hdr(MT_CREATE_ACCOUNT_REQUEST_ID, sizeof(m_Request), wClientID, g_dwSequenceNumber++, dwClientContext),
          m_Request(szFirstName, szLastName, szEmailAddress, wPin, dwSSN, dwDLN)
    { };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t get_Size(void) const
    { return sizeof(*this); };

/// ===================================================
/// Server Decoding Helper Methods

    DWORD      get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD       get_ClientID(void) const
    { return m_Hdr.get_ClientID(); };

    DWORD      get_Sequence(void) const
    { return m_Hdr.get_Sequence(); };

    DWORD       get_Context(void) const
    { return m_Hdr.get_Context(); };

    const char* get_FirstName(void) const
    { return m_Request.m_szFirstName; };

    const char* get_LastName(void) const
    { return m_Request.m_szLastName; };

    const char* get_EmailAddress(void) const
    { return m_Request.m_szEmailAddress; };
    
    WORD        get_PIN(void) const
    { return m_Request.m_wPIN; };

    DWORD       get_SSNumber(void) const
    { return m_Request.m_dwSSNumber; };

    DWORD       get_DLNumber(void) const
    { return m_Request.m_dwDLNumber; };
};

/// [Server] Create Account Response message
struct CREATE_ACCOUNT_RESPONSE
{
    STD_HDR                          m_Hdr;
    prim::_CREATE_ACCOUNT_RESPONSE   m_Response;

/// Default constructor
    CREATE_ACCOUNT_RESPONSE()
        :m_Hdr(),
         m_Response()
    { };

/// Initialization Constructor
    CREATE_ACCOUNT_RESPONSE(DWORD dwResult,         ///< Server generated CER result
                            WORD  wClientID,        ///< Copied from Client's _REQUEST msg
                            DWORD dwSequenceNumber, ///< Copied from Client's _REQUEST msg
                            DWORD dwContext)        ///< Copied from Client's _REQUEST msg
        : m_Hdr(MT_CREATE_ACCOUNT_RESPONSE_ID, sizeof(m_Response), wClientID, dwSequenceNumber, dwContext),
          m_Response(dwResult)
    { };

    DWORD    get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    DWORD    get_ResponseResult(void) const
    { return m_Response.m_dwResult; };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t   get_Size(void) const
    { return sizeof(*this); };
};

/// [Client] Logon Request message
struct LOGON_REQUEST
{
    STD_HDR                 m_Hdr;
    prim::_LOGON_REQUEST    m_Request;

    /// Default constructor
    LOGON_REQUEST()
        : m_Hdr(MT_LOGON_REQUEST_ID, sizeof(m_Request), 0, g_dwSequenceNumber++, 0),
          m_Request()
    { };

    /// Initialization constructor
    LOGON_REQUEST(WORD wClientID,         ///< Server provided Client ID
                  QWORD qwPAN,
                  WORD  wPIN, 
                  DWORD dwContext = 0)    ///< Option Client Field
        : m_Hdr(MT_LOGON_REQUEST_ID, sizeof(m_Request), wClientID, g_dwSequenceNumber++, dwContext ),
          m_Request(qwPAN, wPIN)
    { };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t get_Size(void) const
    { return sizeof(*this); };

/// ===================================================
/// Server Decoding Helper Methods

    DWORD      get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD       get_ClientID(void) const
    { return m_Hdr.get_ClientID(); };

    DWORD      get_Sequence(void) const
    { return m_Hdr.get_Sequence(); };

    DWORD       get_Context(void) const
    { return m_Hdr.get_Context(); };

    QWORD get_PAN(void) const
    { return m_Request.m_PAN; };
    
    WORD        get_PIN(void) const
    { return m_Request.m_wPIN; };

};

/// [Server] Logon Response message
struct LOGON_RESPONSE
{
    STD_HDR                    m_Hdr;
    prim::_LOGON_RESPONSE      m_Response;

    /// Default Constructor
    LOGON_RESPONSE()
        : m_Hdr(),
          m_Response()
    { };

    LOGON_RESPONSE(DWORD dwResult,         ///< Server generated CER result
                   WORD  wClientID,        ///< Copied from Client's _REQUEST msg
                   DWORD dwSequenceNumber, ///< Copied from Client's _REQUEST msg
                   DWORD dwContext)        ///< Copied from Client's _REQUEST msg
       : m_Hdr(MT_LOGOFF_RESPONSE_ID, sizeof(m_Response), wClientID, dwSequenceNumber, dwContext),
         m_Response(dwResult)
    { };

    DWORD  get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    DWORD  get_ResponseResult(void) const
    { return m_Response.m_dwResult; };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t get_Size(void) const
    { return sizeof(*this); };

};

/// [Client] Logoff Request message
struct LOGOFF_REQUEST
{
    STD_HDR                    m_Hdr;
    prim::_LOGOFF_REQUEST      m_Request;

    LOGOFF_REQUEST()
        : m_Hdr(),
          m_Request()
    { };

    /// Initialization constructor
    LOGOFF_REQUEST(WORD  wClientID,      ///< Server generated Client ID (returned in CONNECT_RESPONSE msg
                   DWORD dwContext = 0)  ///< Client provided optional field
        : m_Hdr(MT_LOGOFF_REQUEST_ID, sizeof(m_Request), wClientID, g_dwSequenceNumber++, dwContext),
          m_Request()
    { };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t    get_Size(void) const
    { return sizeof(*this); };

    /// ===================================================
/// Server Decoding Helper Methods

    DWORD      get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD       get_ClientID(void) const
    { return m_Hdr.get_ClientID(); };

    DWORD      get_Sequence(void) const
    { return m_Hdr.get_Sequence(); };

    DWORD       get_Context(void) const
    { return m_Hdr.get_Context(); };

};

/// [Server] Logoff Response message
struct LOGOFF_RESPONSE
{
    STD_HDR                m_Hdr;
    prim::_LOGOFF_RESPONSE     m_Response;

    LOGOFF_RESPONSE()
        :m_Hdr(),
         m_Response()
    { };

    LOGOFF_RESPONSE(DWORD dwResult,          ///< Server generated CER result
                    WORD  wClientID,         ///< Copied from Client's _REQUEST msg
                    DWORD dwSequenceNumber,  ///< Copied from Client's _REQUEST msg
                    DWORD dwContext)         ///< Copied from Client's _REQUEST msg
       : m_Hdr(MT_LOGOFF_RESPONSE_ID, sizeof(m_Response), wClientID, dwSequenceNumber, dwContext),
         m_Response(dwResult)
    { };

    DWORD get_MsgType(void) const
    { return m_Hdr.m_dwMsgType; };

    DWORD get_ResponseResult(void) const
    { return m_Response.m_dwResult; };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t get_Size(void) const
    { return sizeof(*this); };
};

/// [Client] Deposit Request message
struct DEPOSIT_REQUEST
{
    STD_HDR                    m_Hdr;
    prim::_DEPOSIT_REQUEST     m_Request;

    DEPOSIT_REQUEST()
            : m_Hdr(MT_DEPOSIT_REQUEST_ID, sizeof(m_Request), 0, g_dwSequenceNumber++ , 0),
              m_Request()
    { };

    /// Initialization constructor
    DEPOSIT_REQUEST(WORD  wClientID,     
                    DWORD dwAmount, 
                    DEPOSIT_TYPE Type, 
                    DWORD dwContext = 0)
        : m_Hdr(MT_DEPOSIT_REQUEST_ID, sizeof(m_Request), wClientID, g_dwSequenceNumber++ , dwContext),
          m_Request(dwAmount, Type)
    { };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t    get_Size(void) const
    { return sizeof(m_Hdr) + sizeof(m_Request); };

/// ===================================================
/// Server Decoding Helper Methods

    DWORD      get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD       get_ClientID(void) const
    { return m_Hdr.get_ClientID(); };

    DWORD      get_Sequence(void) const
    { return m_Hdr.get_Sequence(); };

    DWORD       get_Context(void) const
    { return m_Hdr.get_Context(); };

    DWORD       get_Amount(void) const
    { return m_Request.m_dwAmount; };
    
    WORD        get_DepositType(void) const
    { return m_Request.m_wType; };
};

/// [Server] Deposit Response message
struct DEPOSIT_RESPONSE
{
    STD_HDR                    m_Hdr;
    prim::_DEPOSIT_RESPONSE    m_Response;

    DEPOSIT_RESPONSE()
        : m_Hdr(),
          m_Response()
    { };

    DEPOSIT_RESPONSE(DWORD dwResult,         ///< Server generated CER result
                     WORD  wClientID,        ///< Copied from Client's _REQUEST msg
                     DWORD dwSequenceNumber, ///< Copied from Client's _REQUEST msg
                     DWORD dwContext)        ///< Copied from Client's _REQUEST msg
        : m_Hdr(MT_DEPOSIT_RESPONSE_ID, sizeof(m_Response), wClientID, dwSequenceNumber, dwContext),
          m_Response(dwResult)
    { };

    DWORD get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    DWORD get_ResponseResult(void) const
    { return m_Response.m_dwResult; };

    size_t get_Size(void) const
    { return sizeof(*this); };
};

/// [Client] Withdrawal Request message
struct WITHDRAWAL_REQUEST
{
    STD_HDR                    m_Hdr;
    prim::_WITHDRAWAL_REQUEST  m_Request;

    WITHDRAWAL_REQUEST()
            :m_Hdr(MT_WITHDRAWAL_REQUEST_ID, sizeof(m_Request), 0, g_dwSequenceNumber++, 0 ),
             m_Request()
    { };

    /// Initialization constructor
    WITHDRAWAL_REQUEST(WORD  wClientID, 
                       DWORD dwAmount, 
                       DWORD dwContext = 0)
        : m_Hdr(MT_WITHDRAWAL_REQUEST_ID, sizeof(m_Request), wClientID, g_dwSequenceNumber++, dwContext ),
          m_Request(dwAmount)
    { };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t   get_Size(void) const
    { return sizeof(m_Hdr) + sizeof(m_Request); };

/// ===================================================
/// Server Decoding Helper Methods

    DWORD      get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD       get_ClientID(void) const
    { return m_Hdr.get_ClientID(); };

    DWORD      get_Sequence(void) const
    { return m_Hdr.get_Sequence(); };

    DWORD       get_Context(void) const
    { return m_Hdr.get_Context(); };

    DWORD       get_Amount(void) const
    { return m_Request.m_dwAmount; };

};

/// [Server] Withdrawal Response message
struct WITHDRAWAL_RESPONSE
{
    STD_HDR                      m_Hdr;
    prim::_WITHDRAWAL_RESPONSE   m_Response;

    WITHDRAWAL_RESPONSE(DWORD dwResult,          ///< Server generated CER result
                        WORD  wClientID,         ///< Copied from _REQUEST msg
                        DWORD dwSequenceNumber,  ///< Copied from _REQUEST msg
                        DWORD dwContext)         ///< Copied from _REQUEST msg
      :m_Hdr(MT_WITHDRAWAL_RESPONSE_ID, sizeof(m_Response), wClientID, dwSequenceNumber, dwContext),
       m_Response(dwResult)
    { };

    WITHDRAWAL_RESPONSE(): m_Hdr(), m_Response()
    {};

    DWORD    get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    DWORD    get_ResponseResult(void) const
    { return m_Response.m_dwResult; };

    size_t   get_Size(void) const
    { return sizeof(*this); };
};

/// [Client] Balance Query Request message
struct BALANCE_QUERY_REQUEST
{
    STD_HDR                        m_Hdr;
    prim::_BALANCE_QUERY_REQUEST   m_Request;

    /// Default constructor
    BALANCE_QUERY_REQUEST()
        : m_Hdr(MT_BALANCE_QUERY_REQUEST_ID, sizeof(m_Request), 0, g_dwSequenceNumber++, 0),
          m_Request()
    { };

    /// Initialization constructor
    BALANCE_QUERY_REQUEST(WORD  wClientID,      ///< Server generated Client ID (previously returned in CONNECT_RESPONSE msg
                          DWORD dwContext = 0)  ///< Client provided optional field
        : m_Hdr(MT_BALANCE_QUERY_REQUEST_ID, sizeof(m_Request), wClientID, g_dwSequenceNumber++, dwContext),
          m_Request()
    { };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t   get_Size(void) const
    { return sizeof(*this); };

/// ===================================================
/// Server Decoding Helper Methods

    DWORD      get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD       get_ClientID(void) const
    { return m_Hdr.get_ClientID(); };

    DWORD      get_Sequence(void) const
    { return m_Hdr.get_Sequence(); };

    DWORD       get_Context(void) const
    { return m_Hdr.get_Context(); };

};

/// [Server] Balance Query response message
struct BALANCE_QUERY_RESPONSE
{
    STD_HDR                          m_Hdr;
    prim::_BALANCE_QUERY_RESPONSE    m_Response;

    BALANCE_QUERY_RESPONSE(DWORD dwResult,         ///< Server generated CER result
                           WORD  wClientID,        ///< Copied from Client's _REQUEST msg
                           DWORD dwBalance,        ///< Client account balance
                           DWORD dwContext,        ///< Copied from Client's _REQUEST msg
                           DWORD dwSequenceNumber) ///< Copied from Client's _REQUEST msg
        : m_Hdr(MT_BALANCE_QUERY_RESPONSE_ID, sizeof(m_Response), dwContext, dwSequenceNumber),
          m_Response(dwResult, dwBalance)
    { };

    DWORD     get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    DWORD     get_ResponseResult(void) const
    { return m_Response.m_dwResult; };

    size_t    get_Size(void) const
    { return sizeof(*this); };
};

/// [Client] Transaction Query Request message
struct TRANSACTION_QUERY_REQUEST
{
    STD_HDR                           m_Hdr;
    prim::_TRANSACTION_QUERY_REQUEST  m_Request;

    TRANSACTION_QUERY_REQUEST()
            :m_Hdr(MT_TRANSACTION_QUERY_REQUEST_ID, sizeof(m_Request), 0, g_dwSequenceNumber++, 0),
             m_Request()
    { };

    /// Initialization constructor
    TRANSACTION_QUERY_REQUEST(WORD  wClientID,
                              DWORD dwStartID, 
                              WORD  wTransactionCount,
                              DWORD dwContext = 0)
        : m_Hdr(MT_TRANSACTION_QUERY_REQUEST_ID, sizeof(m_Request), wClientID, g_dwSequenceNumber++, dwContext),
          m_Request(dwStartID, wTransactionCount)
    { };

/**
    @retval size_t containing the size of the message in bytes
*/
    size_t get_Size(void) const
    { return sizeof(*this); };

/// ===================================================
/// Server Decoding Helper Methods

    DWORD      get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD       get_ClientID(void) const
    { return m_Hdr.get_ClientID(); };

    DWORD      get_Sequence(void) const
    { return m_Hdr.get_Sequence(); };

    DWORD       get_Context(void) const
    { return m_Hdr.get_Context(); };

    DWORD       get_StartID(void) const
    { return m_Request.m_dwStartID; };

    WORD        get_TransactionCount(void) const
    { return m_Request.m_wTransactionCount; };
};

/// [Server] Transaction Query Response message
struct TRANSACTION_QUERY_RESPONSE
{
    STD_HDR                            m_Hdr;
    prim::_TRANSACTION_QUERY_RESPONSE  m_Response;


    TRANSACTION_QUERY_RESPONSE():m_Hdr(), m_Response()
    {};

    TRANSACTION_QUERY_RESPONSE(DWORD dwResult,
                               WORD  wClientID,
                               WORD  wTransactionCount,
                               DWORD dwSequenceNumber,
                               DWORD dwContext)        ///< [optional field] copied from Client's _REQUEST msg
      : m_Hdr(MT_TRANSACTION_QUERY_RESPONSE_ID, sizeof(m_Response), wClientID, dwSequenceNumber, dwContext),
        m_Response(dwResult, wTransactionCount)
    {};

    DWORD get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    DWORD get_ResponseResult(void) const
    { return m_Response.m_dwResult; };
};

/// [Client] Stamp Purchase Request Message
struct STAMP_PURCHASE_REQUEST
{
    STD_HDR                       m_Hdr;
    prim::_STAMP_PURCHASE_REQUEST m_Request;

    /// Initialization constructor
    STAMP_PURCHASE_REQUEST(WORD  wClientID,      ///< Server generated Client ID (returned in CONNECT_RESPONSE msg
                           DWORD dwAmount,       ///< Cost of stamps attempting to purchase (in cents) (i.e. 1000 = $10.00)
                           DWORD dwContext = 0)  ///< [optional field] provided by the Client
        : m_Hdr(MT_PURCHASE_STAMPS_REQUEST_ID, sizeof(m_Request), wClientID, g_dwSequenceNumber++, dwContext),
          m_Request(dwAmount)
    { };

    size_t   get_Size(void) const
    { return sizeof(*this); };

/// ===================================================
/// Server Decoding Helper Methods

    DWORD      get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };

    WORD       get_ClientID(void) const
    { return m_Hdr.get_ClientID(); };

    DWORD      get_Sequence(void) const
    { return m_Hdr.get_Sequence(); };

    DWORD       get_Context(void) const
    { return m_Hdr.get_Context(); };

    DWORD       get_Amount(void) const
    { return m_Request.m_dwAmount; };

};

/// [Server] Stamp Purchase Response Message
struct STAMP_PURCHASE_RESPONSE
{
    STD_HDR                         m_Hdr;
    prim::_STAMP_PURCHASE_RESPONSE  m_Response;

    STAMP_PURCHASE_RESPONSE(DWORD dwResult,          ///< Server generated result
                            WORD  wClientID,         ///< Copied from Client's _REQUEST msg
                            DWORD dwSequenceNumber,  ///< Copied from Client's _REQUEST msg
                            DWORD dwContext)         ///< [optional field] Copied from Client's _REQUEST msg
        : m_Hdr(MT_PURCHASE_STAMPS_RESPONSE_ID, sizeof(m_Response), wClientID, dwSequenceNumber, dwContext),
          m_Response(dwResult)
    { };

    DWORD  get_MsgType(void) const
    { return m_Hdr.get_MsgType(); };
    
    DWORD  get_ResponseResult(void) const
    { return m_Response.m_dwResult; };

    size_t get_Size(void) const
    { return sizeof(*this); };
};

} // namespace cnp

// restore the default structure alignment
#pragma pack(pop)

#endif
