/*
 * @brief:重新优化json，提升效率
 */
#include "Json_data.hpp"

namespace Json
{
	CJsonData::CJsonData()
	{

	}

	CJsonData::CJsonData(rapidjson::Type rDataType)
		:
		mAlloc(new rapidjson::Value::AllocatorType()),
		mValue(new rapidjson::Value(rDataType))
	{

	}

	CJsonData::~CJsonData()
	{

	}

	CJsonData::CJsonData(std::shared_ptr<rapidjson::Value::AllocatorType> &alloc,
		const rapidjson::Value& rValue)
	{
		mAlloc = alloc;
		mValue.reset(new rapidjson::Value(rValue, *mAlloc, true));
	}

	void CJsonData::Init()
	{
		if ( !mAlloc ) {
			mAlloc.reset(new rapidjson::Value::AllocatorType());
		}

		if ( !mValue ) {
			mValue.reset(new rapidjson::Value());
		}
	}

	//get
	bool CJsonData::GetValueByName(const std::string& rstrName, std::string& rstrValue)//值为字符串
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ConstObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		if ( !it->value.IsString() ) {
			return false;
		}

		rstrValue = it->value.GetString();

		return true;

	}

	bool CJsonData::GetValueByName(const std::string& rstrName, char*& pValue)//值为字符串
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ConstObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		if ( !it->value.IsString() ) {
			return false;
		}

		strcpy(pValue, it->value.GetString());

		return true;
	}

	/*值为布尔类型*/
	bool CJsonData::GetValueByName(const std::string& rstrName, bool& rbValue)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ConstObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		if ( it->value.IsBool() ) {
			rbValue = it->value.GetBool();
			return true;
		}

		if ( it->value.IsNumber() ) {
			rbValue = (it->value.GetDouble() == 0.0 ? false : true);
			return true;
		}
		
		return false;
	}

	bool CJsonData::GetValueByName(const std::string& rstrName, int32_t& riValue)//值为int
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ConstObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		if ( !it->value.IsNumber() ) {
			return false;
		}

		riValue = static_cast<int32_t>(it->value.GetDouble());

		return true;
	}

	bool CJsonData::GetValueByName(const std::string& rstrName, long& rlValue)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ConstObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		if ( it->value.IsInt64() ) {
			rlValue = static_cast<long>(it->value.GetInt64());
			return true;
		}

		if ( it->value.IsUint64() ) {
			rlValue = static_cast<long>(it->value.GetUint64());
			return true;

		}

		if ( it->value.IsNumber() ) {
			rlValue = static_cast<long>(it->value.GetDouble());
			return true;
		}

		return false;
	}

	bool CJsonData::GetValueByName(const std::string& rstrName, float& rfValue)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ConstObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		if ( !it->value.IsNumber() ) {
			return false;
		}

		rfValue = static_cast<float>(it->value.GetDouble());
		return true;
	}

	bool CJsonData::GetValueByName(const std::string& rstrName, double& rdValue)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ConstObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		if ( !it->value.IsNumber() ) {
			return false;
		}

		rdValue = it->value.GetDouble();
		return true;
	}

	bool CJsonData::GetValueByName(const std::string& rstrName, CJsonData& rDataCon)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ConstObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		rDataCon = CJsonData(mAlloc, it->value);
		return true;
	}

	//针对数组类型的
	//数组值为字符串
	bool CJsonData::GetValueByIndex(uint32_t ruIndex, std::string& rstrValue)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		const rapidjson::Value &value = (*mValue)[ruIndex];

		if ( !value.IsString() ) {
			return false;
		}

		rstrValue = value.GetString();

		return true;
	}

	//数组值为int
	bool CJsonData::GetValueByIndex(uint32_t ruIndex, int32_t& riValue)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		const rapidjson::Value &value = (*mValue)[ruIndex];

		if ( !value.IsNumber() ) {
			return false;
		}

		riValue = static_cast<int32_t>(value.GetDouble());

		return true;
	}

	//数组值为long
	bool CJsonData::GetValueByIndex(uint32_t ruIndex, long& rlValue)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		const rapidjson::Value &value = (*mValue)[ruIndex];

		if ( value.IsInt64() ) {
			rlValue = static_cast<long>(value.GetInt64());
			return true;
		}

		if ( value.IsUint64() ) {
			rlValue = static_cast<long>(value.GetUint64());
			return true;

		}

		if ( value.IsNumber() ) {
			rlValue = static_cast<long>(value.GetDouble());
			return true;
		}

		return false;
	}

	//数组值为float
	bool CJsonData::GetValueByIndex(uint32_t ruIndex, float& rfValue)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		const rapidjson::Value &value = (*mValue)[ruIndex];

		if ( !value.IsNumber() ) {
			return false;
		}

		rfValue = static_cast<float>(value.GetDouble());

		return true;
	}

	//数组值为double
	bool CJsonData::GetValueByIndex(uint32_t ruIndex, double& rdValue)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		const rapidjson::Value &value = (*mValue)[ruIndex];

		if ( !value.IsNumber() ) {
			return false;
		}

		rdValue = value.GetDouble();

		return true;
	}

	//数组的值为CJsonData对象
	bool CJsonData::GetValueByIndex(uint32_t ruIndex, CJsonData& rData)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		const rapidjson::Value &value = (*mValue)[ruIndex];

		rData = CJsonData(mAlloc, value);

		return true;
	}

	//返回数量
	uint32_t CJsonData::GetSize()
	{
		if ( !mValue ) {
			return 0;
		}

		if ( mValue->IsArray() ) {
			return mValue->Size();
		}

		if ( mValue->IsObject() ) {
			return mValue->MemberCount();
		}

		return 0;
	}

	/*update*/
	bool CJsonData::UpdateValueByName(const std::string& rstrName, 
		const std::string& rstrValue)	/*值为字符串*/
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		Init();

		rapidjson::Value value(rstrValue.c_str(), *mAlloc);
		it->value = value;

		return true;
	}

	bool CJsonData::UpdateValueByName(const std::string& rstrName, 
		const char* pValue)			/*值为字符串*/
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		Init();

		if ( pValue == NULL ) {
			rapidjson::Value value("", *mAlloc);
			it->value = value;
		}
		else {
			rapidjson::Value value(pValue, *mAlloc);
			it->value = value;
		}

		return true;
	}

	bool CJsonData::UpdateValueByName(const std::string& rstrName, 
		bool rbValue)			/*值为布尔类型（bool）*/
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		rapidjson::Value value(rbValue);
		it->value = value;

		return true;
	}

	bool CJsonData::UpdateValueByName(const std::string& rstrName, 
		int32_t riValue)		/*值为int*/
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		rapidjson::Value value(riValue);
		it->value = value;

		return true;
	}

	bool CJsonData::UpdateValueByName(const std::string& rstrName, 
		long rlValue)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		rapidjson::Value value((int64_t)rlValue);
		it->value = value;

		return true;
	}

	bool CJsonData::UpdateValueByName(const std::string& rstrName, 
		float rfValue)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		rapidjson::Value value(rfValue);
		it->value = value;

		return true;
	}

	bool CJsonData::UpdateValueByName(const std::string& rstrName, 
		double rdValue)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}
		
		rapidjson::Value value(rdValue);
		it->value = value;

		return true;
	}

	bool CJsonData::UpdateValueByName(const std::string& rstrName, 
		const CJsonData& rDataCon)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		ObjectIterator it = mValue->FindMember(rstrName.c_str());
		if ( it == mValue->MemberEnd() ) {
			return false;
		}

		Init();

		rapidjson::Value value(*rDataCon.mValue, *mAlloc, true);
		it->value = value;

		return true;
	}

	/*更新数组里面的内容update value by index*/
	bool CJsonData::UpdateValueByIndex(uint32_t ruIndex, const std::string& rstrValue)	/*值为字符串*/
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		Init();

		rapidjson::Value value(rstrValue.c_str(), *mAlloc);
		(*mValue)[ruIndex] = value;

		return true;
	}

	bool CJsonData::UpdateValueByIndex(uint32_t ruIndex, const char* pValue)			/*值为字符串*/
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		Init();

		if ( pValue == NULL ) {
			rapidjson::Value value("", *mAlloc);
			(*mValue)[ruIndex] = value;
		}
		else {
			rapidjson::Value value(pValue, *mAlloc);
			(*mValue)[ruIndex] = value;
		}

		return true;
	}

	bool CJsonData::UpdateValueByIndex(uint32_t ruIndex, bool rbValue)			/*值为布尔类型（bool）*/
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		rapidjson::Value value(rbValue);
		(*mValue)[ruIndex] = value;

		return true;
	}

	bool CJsonData::UpdateValueByIndex(uint32_t ruIndex, int32_t riValue)		/*值为int*/
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		rapidjson::Value value(riValue);
		(*mValue)[ruIndex] = value;

		return true;
	}

	bool CJsonData::UpdateValueByIndex(uint32_t ruIndex, long rlValue)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		rapidjson::Value value((int64_t)rlValue);
		(*mValue)[ruIndex] = value;

		return true;
	}

	bool CJsonData::UpdateValueByIndex(uint32_t ruIndex, float rfValue)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		rapidjson::Value value(rfValue);
		(*mValue)[ruIndex] = value;

		return true;
	}

	bool CJsonData::UpdateValueByIndex(uint32_t ruIndex, double rdValue)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		rapidjson::Value value(rdValue);
		(*mValue)[ruIndex] = value;

		return true;
	}

	bool CJsonData::UpdateValueByIndex(uint32_t ruIndex, const CJsonData& rDataCon)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}

		if ( ruIndex >= mValue->Size() ) {
			return false;
		}

		Init();

		rapidjson::Value value(*rDataCon.mValue, *mAlloc, true);
		(*mValue)[ruIndex] = value;

		return true;
	}

	//set
	//值为字符串
	bool CJsonData::CheckPush(const std::string &rstrName)
	{
		if ( !mValue ) {
			Init();
		}

		if ( mValue->IsNull() ) {
			if ( rstrName.empty() ) {
				mValue->SetArray();
			}
			else {
				mValue->SetObject();
			}
		}

		if ( rstrName.empty() && !mValue->IsArray() ) {
			return false;
		}

		if ( !rstrName.empty() && !mValue->IsObject() ) {
			return false;
		}

		return true;
	}

	bool CJsonData::PushValue(const std::string& rstrValue)
	{
		if ( !CheckPush("") ) {
			return false;
		}

		rapidjson::Value item(rstrValue.c_str(), *mAlloc);
		mValue->PushBack(item, *mAlloc);

		return true;
	}

	bool CJsonData::PushValue(const std::string& rstrName, const std::string& rstrValue)
	{
		if ( !CheckPush(rstrName) ) {
			return false;
		}

		rapidjson::Value name(rstrName.c_str(), *mAlloc);
		rapidjson::Value value(rstrValue.c_str(), *mAlloc);
		mValue->AddMember(name, value, *mAlloc);

		return true;
	}

	bool CJsonData::PushValue(const char* pValue)	/*值为字符串*/
	{
		if ( !CheckPush("") ) {
			return false;
		}

		if ( pValue == NULL ) {
			rapidjson::Value item("", *mAlloc);
			mValue->PushBack(item, *mAlloc);
		}
		else {
			rapidjson::Value item(pValue, *mAlloc);
			mValue->PushBack(item, *mAlloc);
		}

		return true;
	}

	bool CJsonData::PushValue(const std::string& rstrName, const char* pValue)	/*值为字符串*/
	{
		if ( !CheckPush(rstrName) ) {
			return false;
		}

		if ( pValue == NULL ) {
			rapidjson::Value name(rstrName.c_str(), *mAlloc);
			rapidjson::Value value("", *mAlloc);
			mValue->AddMember(name, value, *mAlloc);
		}
		else {
			rapidjson::Value name(rstrName.c_str(), *mAlloc);
			rapidjson::Value value(pValue, *mAlloc);
			mValue->AddMember(name, value, *mAlloc);
		}

		return true;
	}

	/*值为布尔类型*/
	bool CJsonData::PushValue(bool rbValue)
	{
		if ( !CheckPush("") ) {
			return false;
		}

		rapidjson::Value item(rbValue);
		mValue->PushBack(item, *mAlloc);


		return true;
	}

	/*值为布尔类型*/
	bool CJsonData::PushValue(const std::string& rstrName, bool rbValue)
	{
		if ( !CheckPush(rstrName) ) {
			return false;
		}

		rapidjson::Value name(rstrName.c_str(), *mAlloc);
		rapidjson::Value value(rbValue);
		mValue->AddMember(name, value, *mAlloc);

		return true;
	}

	//值为int
	bool CJsonData::PushValue(int32_t riValue)
	{
		if ( !CheckPush("") ) {
			return false;
		}

		rapidjson::Value item(riValue);
		mValue->PushBack(item, *mAlloc);


		return true;
	}

	//值为int
	bool CJsonData::PushValue(const std::string& rstrName, int32_t riValue)
	{
		if ( !CheckPush(rstrName) ) {
			return false;
		}

		rapidjson::Value name(rstrName.c_str(), *mAlloc);
		rapidjson::Value value(riValue);
		mValue->AddMember(name, value, *mAlloc);

		return true;
	}

	bool CJsonData::PushValue(long rlValue)
	{
		if ( !CheckPush("") ) {
			return false;
		}

		rapidjson::Value item((int64_t)rlValue);
		mValue->PushBack(item, *mAlloc);

		return true;
	}

	bool CJsonData::PushValue(const std::string& rstrName, long rlValue)
	{
		if ( !CheckPush(rstrName) ) {
			return false;
		}

		rapidjson::Value name(rstrName.c_str(), *mAlloc);
		rapidjson::Value value((int64_t)rlValue);
		mValue->AddMember(name, value, *mAlloc);

		return true;
	}

	bool CJsonData::PushValue(float rfValue)
	{
		if ( !CheckPush("") ) {
			return false;
		}

		rapidjson::Value item(rfValue);
		mValue->PushBack(item, *mAlloc);

		return true;
	}

	bool CJsonData::PushValue(const std::string& rstrName, float rfValue)
	{
		if ( !CheckPush(rstrName) ) {
			return false;
		}

		rapidjson::Value name(rstrName.c_str(), *mAlloc);
		rapidjson::Value value(rfValue);
		mValue->AddMember(name, value, *mAlloc);

		return true;
	}

	/*添加double类型的数据*/
	bool CJsonData::PushValue(double rdValue)
	{
		if ( !CheckPush("") ) {
			return false;
		}

		rapidjson::Value item(rdValue);
		mValue->PushBack(item, *mAlloc);

		return true;
	}

	/*添加double类型的数据*/
	bool CJsonData::PushValue(const std::string& rstrName, double rdValue)
	{
		if ( !CheckPush(rstrName) ) {
			return false;
		}

		rapidjson::Value name(rstrName.c_str(), *mAlloc);
		rapidjson::Value value(rdValue);
		mValue->AddMember(name, value, *mAlloc);

		return true;
	}

	/*添加CJsonData类型的数据*/
	bool CJsonData::PushValue(const CJsonData& rDataCon)
	{
		if ( !CheckPush("") ) {
			return false;
		}

		rapidjson::Value item(*rDataCon.mValue, *mAlloc, true);
		mValue->PushBack(item, *mAlloc);

		return true;
	}

	/*添加CJsonData类型的数据*/
	bool CJsonData::PushValue(const std::string& rstrName, const CJsonData& rDataCon)
	{
		if ( !CheckPush(rstrName) ) {
			return false;
		}

		rapidjson::Value name(rstrName.c_str(), *mAlloc);
		rapidjson::Value value(*rDataCon.mValue, *mAlloc, true);
		mValue->AddMember(name, value, *mAlloc);

		return true;
	}

	/*删除指定名称的json*/
	bool CJsonData::RemoveValue(const std::string& rstrName)
	{
		if ( !mValue || rstrName.empty() ) {
			return false;
		}

		if ( !mValue->EraseMember(rstrName.c_str()) ) {
			return false;
		}

		return true;
	}

	void CJsonData::EraseAll(void)
	{
		if ( !mValue ) {
			return;
		}

		mValue->SetNull();
	}

	/*
	* @function:decodeJson，
	* @param:
	* rstrJson:std::string,输入要解析的json数据
	* @return
	* 0:解析成功
	* -1:解析失败
	*/
	bool CJsonData::DecodeJson(const std::string& rstrJson)
	{
		if ( rstrJson.empty() ) {
			return false;
		}

		Init();

		rapidjson::Document dataDoc(mAlloc.get());
		dataDoc.Parse<0>(rstrJson.c_str());
		if ( dataDoc.HasParseError() ) {
			return false;
		}

		*mValue = *static_cast<rapidjson::Value*>(&dataDoc);

		return true;
	}

	/*
	* @function:encodeJson,封装数据，将CJsonData的数据转换为std::string类型
	* @param:
	* rstrJson:std::string,输出的转换后的json字符串
	* @return:
	* 0:封装成功
	* -1:封装失败
	*/
	bool CJsonData::EncodeJson(std::string& rstrJson)
	{
		if ( !mValue ) {
			return false;
		}

		//转换为字符串
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		mValue->Accept(writer);

		rstrJson = buffer.GetString();

		return true;
	}

	/*
	* @function:EncodeArryJson,封装数据，将CJsonData的数据转换为std::string类型,[]
	* @param:
	* rstrArrayJson:std::string,输出的转换后的json字符串
	* @return:
	* 0:封装成功
	* -1:封装失败
	*/
	bool CJsonData::EncodeArryJson(std::string& rstrArrayJson)
	{
		if ( !mValue || !mValue->IsArray() ) {
			return false;
		}
		
		return EncodeJson(rstrArrayJson);
	}

	/*判断是否包含子项rstrName*/
	bool CJsonData::IsHasMember(const std::string& rstrName)
	{
		if ( !mValue || !mValue->IsObject() ) {
			return false;
		}

		return mValue->HasMember(rstrName.c_str());
	}

	/* 对象迭代 */
	CJsonData::ObjectIterator CJsonData::ObjectBegin()
	{
		if ( !mValue || !mValue->IsObject() ) {
			return ObjectEnd();
		}

		return mValue->MemberBegin();
	}

	CJsonData::ObjectIterator CJsonData::ObjectEnd()
	{
		if ( !mValue || !mValue->IsObject() ) {
			return ObjectIterator();
		}

		return mValue->MemberEnd();
	}

	CJsonData::ConstObjectIterator CJsonData::ObjectBegin() const
	{
		if ( !mValue || !mValue->IsObject() ) {
			return ObjectEnd();
		}

		return mValue->MemberBegin();
	}

	CJsonData::ConstObjectIterator CJsonData::ObjectEnd() const
	{
		if ( !mValue || !mValue->IsObject() ) {
			return ObjectIterator();
		}

		return mValue->MemberEnd();	
	}

	/* 数组迭代器 */
	CJsonData::ArrayIterator CJsonData::ArrayBegin()
	{
		if ( !mValue || !mValue->IsArray() ) {
			return ArrayEnd();
		}

		return mValue->Begin();			
	}

	CJsonData::ArrayIterator CJsonData::ArrayEnd()
	{
		if ( !mValue || !mValue->IsArray() ) {
			return NULL;
		}

		return mValue->End();	
	}

	CJsonData::ConstArrayIterator CJsonData::ArrayBegin() const
	{
		if ( !mValue || !mValue->IsArray() ) {
			return ArrayEnd();
		}

		return mValue->Begin();	
	}

	CJsonData::ConstArrayIterator CJsonData::ArrayEnd() const
	{
		if ( !mValue || !mValue->IsArray() ) {
			return NULL;
		}

		return mValue->End();	
	}

}
