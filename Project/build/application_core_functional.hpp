#pragma once

#include "nlohmann/json.hpp"

//Twilight-Dream's Cryptography Library Base Support

/* Priority Level 1 */
#include "Support+Library/Support-Library.hpp"

#include "raii_tool.hpp"
#include "logger.hpp"

// System User data structure
struct PasswordManagerUserData
{
	//系统用户名，生成全局唯一身份标识的材料之一。
	//System username, one of the materials used to generate a globally unique identifiers.
	std::string UserName;

	// 经过散列加盐和 Base64 编码的系统密码
	// Hash-salted and Base64-encoded system passwords
	std::string HashedPassword;

	std::string PersonalPasswordInfoFileName = "";
	std::string PersonalInfoFileName = "";

	bool IsFirstLogin = true;
};

// System User key structure
struct PasswordManagerUserKey
{
	//全局唯一身份标识，是用户个人密码管理器的主密钥数据的生成材料。
	//A globally unique identifier that is the material from which the master key data for the user's personal password manager is generated.
	std::string RandomUUID;
	
	//关于系统用户名的盐值，生成全局唯一身份标识的材料之一。生成一次之后不会更改。
	//Salt value about the system username, one of the materials for generating a globally unique identifier.
	//It will not be changed after it is generated once.
	std::string RandomSalt;
	
	//关于系统密码的盐值，验证密码时会使用。生成一次之后不会更改。
	//Salt value about the system password, which will be used when verifying the password.
	//It will not be changed after it is generated once.
	std::string RandomPasswordSalt;

	//注册时间戳，生成全局唯一身份标识的材料之一。
	//Registration timestamp, one of the materials for generating globally unique identifiers.
	uint64_t RegistrationTime = 0;
};

const std::vector<std::string> CryptoCipherAlgorithmNames{"AES", "RC6", "SM4", "Twofish", "Serpent"};

// The PersonalPasswordInfo class is used to manage password text instances.
// 用于管理密码文本实例
class PersonalPasswordInfo
{
private:
	struct PersonalPasswordInstance
	{
		// Represents the ID of the PersonalPasswordInstance
		std::uint64_t			 ID = 0;
		std::string				 Description = "";
		std::string				 EncryptedPassword = "";
		std::string				 DecryptedPassword = "";
		std::vector<std::string> EncryptionAlgorithmNames {};
		std::vector<std::string> DecryptionAlgorithmNames {};
		std::uint64_t			 HashMapID = 0;	 // Used for accessing HashMap_EncryptedSymmetricKey and HashMap_DecryptedSymmetricKey_Hashed

		PersonalPasswordInstance() = default;

		PersonalPasswordInstance( const PersonalPasswordInstance& Other )
		: ID( Other.ID ), Description( Other.Description ),
		EncryptedPassword( Other.EncryptedPassword ), DecryptedPassword( Other.DecryptedPassword ),
		EncryptionAlgorithmNames( Other.EncryptionAlgorithmNames ), DecryptionAlgorithmNames( Other.DecryptionAlgorithmNames ),
		HashMapID( Other.HashMapID ) {}

		~PersonalPasswordInstance() = default;
	};

	std::vector<PersonalPasswordInstance> Instances;

	// Recomputes the encrypted password based on the new password and token for the given instance
	void RecomputeEncryptedPassword( const std::string& NewInstancePassword, const std::string& Token, PersonalPasswordInstance& Instance );

	// Recomputes the decrypted password based on the token for the given instance
	void RecomputeDecryptedPassword( const std::string& Token, PersonalPasswordInstance& Instance );

public:
	PersonalPasswordInfo() = default;

	// Copy constructor to create a new instance from another PersonalPasswordInfo object
	explicit PersonalPasswordInfo( const PersonalPasswordInfo& Other ) : Instances( Other.Instances ), HashMap_EncryptedSymmetricKey( Other.HashMap_EncryptedSymmetricKey ), HashMap_DecryptedSymmetricKey_Hashed( Other.HashMap_DecryptedSymmetricKey_Hashed ) {}

	~PersonalPasswordInfo() = default;

	std::unordered_map<std::uint64_t, std::vector<std::uint8_t>> HashMap_EncryptedSymmetricKey;			// Maps HashMapID to EncryptedSymmetricKey
	std::unordered_map<std::uint64_t, std::string>				 HashMap_DecryptedSymmetricKey_Hashed;	// Maps HashMapID to DecryptedSymmetricKey_Hashed

	// Serializes the PersonalPasswordInfo object and saves it to a file
	void Serialization( const std::filesystem::path& FilePath );

	// Deserializes the PersonalPasswordInfo object from a file
	void Deserialization( const std::filesystem::path& FilePath );

	// Creates a new personal password instance and encrypts it
	PersonalPasswordInstance CreatePasswordInstance( const std::string& Token, const std::string& ShowPPI_Description, const std::string& Password, const std::vector<std::string>& EncryptionAlgorithms, const std::vector<std::string>& DecryptionAlgorithms );

	// Appends a new personal password instance
	void AppendPasswordInstance( const PersonalPasswordInfo::PersonalPasswordInstance& instance );

	// Modifies an existing personal password instance
	bool ChangePasswordInstance( std::uint64_t ID, const std::string& NewDescription, const std::string& NewPassword, const std::vector<std::string>& NewEncryptionAlgorithms, const std::vector<std::string>& NewDecryptionAlgorithms, const std::string& Token, bool ChangeEncryptedPassword );

	// Returns a reference to the vector of password instances
	std::vector<PersonalPasswordInstance>& GetPassswordInstances();

	// Lists all personal password instances and decrypts them
	void ListAllPasswordInstance( const std::string& Token );

	// Removes a personal password instance by its ID
	bool RemovePasswordInstance( std::uint64_t id );

	// Removes all personal password instances
	void RemoveAllPasswordInstance();

	// Finds a personal password instance by its ID and decrypts it
	std::optional<PersonalPasswordInstance> FindPasswordInstanceByID( const std::string& Token, std::uint64_t ID );

	// Finds the description of a personal password instance by its ID
	std::string FindPasswordInstanceDescriptionByID( std::uint64_t ID );

	// Finds a personal password instance by its description and decrypts it
	std::optional<PersonalPasswordInstance> FindPasswordInstanceByDescription( const std::string& Token, const std::string& Description );

	// Changes the master key of the instance using the system password
	void ChangeInstanceMasterKeyWithSystemPassword( const std::filesystem::path& FilePath, const std::string& Token, const std::string& NewToken );
};

// The PersonalFileInfo class is used to manage confidential data file instances.
// 用于管理保密数据文件实例
class PersonalFileInfo
{
private:
	// Definition of PersonalFileInstance struct
	struct PersonalFileInstance
	{
		std::uint64_t			 ID = 0;
		std::vector<std::string> EncryptionAlgorithmNames;
		std::vector<std::string> DecryptionAlgorithmNames;

		PersonalFileInstance() = default;

		PersonalFileInstance( const PersonalFileInstance& Other )
		: ID( Other.ID ),
		EncryptionAlgorithmNames( Other.EncryptionAlgorithmNames ), DecryptionAlgorithmNames( Other.DecryptionAlgorithmNames ) {}

		~PersonalFileInstance() = default;
	};

	std::vector<PersonalFileInstance> Instances;

	// Helper functions for serialization and deserialization
	void SerializeInstances( nlohmann::json& jsonData ) const;
	void DeserializeInstances( const nlohmann::json& jsonData );

public:
	PersonalFileInfo() = default;

	// Copy constructor to create a new instance from another PersonalFileInfo object
	explicit PersonalFileInfo( const PersonalFileInfo& Other ) : Instances( Other.Instances ) {}

	~PersonalFileInfo() = default;

	// Serialization and deserialization functions
	void Serialization( const std::filesystem::path& FilePath );
	void Deserialization( const std::filesystem::path& FilePath );

	// Creates a new file instance
	PersonalFileInstance CreateFileInstance( const std::string& Token, const std::vector<std::string>& EncryptionAlgorithms, const std::vector<std::string>& DecryptionAlgorithms );

	// Appends a new file instance
	void AppendFileInstance( const PersonalFileInstance& instance );

	// Removes a file instance by its ID
	bool RemoveFileInstance( std::uint64_t ID );

	// Removes all file instances
	void RemoveAllFileInstances();

	// Retrieves a file instance by its ID
	PersonalFileInstance& GetFileInstanceByID( uint64_t ID );

	/**
     * Encrypts the specified file and saves the encrypted contents to the destination path.
     *
     * @param Token The token string used to generate the master key.
     * @param Instance The instance of the cipher to use.
     * @param SourceFilePath The path to the source file to be encrypted.
     * @param EncryptedFilePath The path where the encrypted file will be saved.
     * @return Returns true if the encryption was successful, otherwise false.
     */
	bool EncryptFile( const std::string& Token, const PersonalFileInstance& Instance, const std::filesystem::path& SourceFilePath, const std::filesystem::path& EncryptedFilePath );

	/**
     * Decrypts the specified encrypted file and saves the decrypted contents to the target path.
     *
     * @param Token The token string used to generate the master key.
     * @param Instance The instance of the cipher to use.
     * @param EncryptedFilePath Path to the source encrypted file to decrypt.
     * @param DecryptedFilePath The path where the decrypted file will be saved.
     * @return Returns true if the decryption was successful, otherwise false.
     */
	bool DecryptFile( const std::string& Token, const PersonalFileInstance& Instance, const std::filesystem::path& EncryptedFilePath, const std::filesystem::path& DecryptedFilePath );
};

// Function to generate a unique user ID
extern inline void GenerateUUID(std::vector<char> UserName, const std::string& RandomSalt, uint64_t& RegistrationTime, std::string& UUID);

// Function to generate a random salt
extern inline void GenerateRandomSalt(std::string& RandomSalt);

// Function to SHA1 hash reduction on the data array
extern std::vector<unsigned char> HashUUID(const std::vector<unsigned char>& data, size_t size_limit);

// Function to Unique user ID hash to Unique file name
extern inline std::string GenerateStringFileUUIDFromStringUUID(const std::string& UUID);

// Function to generate a random key (256 bit)
extern inline std::vector<uint8_t> GenerateRandomKey();

// Function to regenerate a mater key and password instance key
extern inline void RegenerateMasterKey(const std::filesystem::path& FilePath, PersonalPasswordInfo& PersonalPasswordInfo, const std::string& Token);

// Function to save user data to a JSON file
extern inline void SavePasswordManagerUser(const std::pair<PasswordManagerUserKey, PasswordManagerUserData>& NewUserData);

extern inline bool LoadPasswordManagerUUID(PasswordManagerUserKey& CurrentUserKey);

// Function to load user data from a JSON file by UserKey UUID
extern inline void LoadPasswordManagerUser(const PasswordManagerUserKey& CurrentUserKey, PasswordManagerUserData& EmptyUserData);

extern inline std::string PasswordAndHash(const std::vector<char>& Password, std::string RandomSalt);
extern inline std::string PasswordAndHash(const std::string& Password, std::string RandomSalt);

// Function to verify UUID
extern inline bool VerifyUUID(const std::vector<char>& Username, const std::string& RandomSalt, uint64_t& RegistrationTime, const PasswordManagerUserKey& CurrentUserKey);

// Function to verify the password
extern inline bool VerifyPassword(const std::vector<char>& Password, const PasswordManagerUserKey& CurrentUserKey, const PasswordManagerUserData& CurrentUserData);

//UUID string Concatenation Login/Registration Password string = Token String
extern inline std::string MakeTokenString(const std::string& UUID, const std::vector<char>& BufferLoginPassword);
extern inline std::string MakeTokenString(const std::string& UUID, const std::string& BufferLoginPassword);

extern inline std::vector<std::uint8_t> GenerateMasterBytesKeyFromToken(const std::string& Token);

extern inline void MakePersonalPasswordDataFile(const std::string& UniqueFileName, const std::string& Token);

extern void FirstLoginLogic(const std::vector<char>& BufferLoginPassword, const PasswordManagerUserKey& CurrentUserKey, PasswordManagerUserData& CurrentUserData);