#ifndef JSON_JSONDATA_H
#define JSON_JSONDATA_H

#include <vector>
#include <string>
#include <stdint.h>
#include <memory>
#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/prettywriter.h"

namespace Json
{
	class CJsonData
	{
	public:
		/* 对象迭代器 */
		typedef rapidjson::Value::MemberIterator ObjectIterator;
		typedef rapidjson::Value::ConstMemberIterator ConstObjectIterator;

		/* 数组迭代器 */
		typedef rapidjson::Value::ValueIterator ArrayIterator;
		typedef rapidjson::Value::ConstValueIterator ConstArrayIterator;

		/*构造函数*/
		CJsonData();
		CJsonData(rapidjson::Type rDataType);
		/*析构函数*/
		virtual ~CJsonData();

		CJsonData(std::shared_ptr<rapidjson::Value::AllocatorType> &alloc,
			const rapidjson::Value& rValue);

		/*
		* @function:decodeJson，
		* @param:
		* rstrJson:std::string,输入要解析的json数据
		* @return
		* 0:解析成功
		* -1:解析失败
		*/
		bool DecodeJson(const std::string& rstrJson);
		
		/*
		* @function:encodeJson,封装数据，将CJsonData的数据转换为std::string类型
		* @param:			
		* rstrJson:std::string,输出的转换后的json字符串
		* @return:
		* 0:封装成功
		* -1:封装失败
		*/
		bool EncodeJson(std::string& rstrJson);
		/*
		* @function:EncodeArryJson,封装数据，将CJsonData的数据转换为std::string类型
		* @param:
		* rstrArrayJson:std::string,输出的转换后的json字符串
		* @return:
		* 0:封装成功
		* -1:封装失败
		*/
		bool EncodeArryJson(std::string& rstrArrayJson);
		

		void EraseAll(void);/*清除所有内容*/
		
		/*update value by name*/
		bool UpdateValueByName(const std::string& rstrName, const std::string& rstrValue);	/*值为字符串*/
		bool UpdateValueByName(const std::string& rstrName, const char* pValue);			/*值为字符串*/
		bool UpdateValueByName(const std::string& rstrName, bool rbValue);			/*值为布尔类型（bool）*/
		bool UpdateValueByName(const std::string& rstrName, int32_t riValue);		/*值为int*/
		bool UpdateValueByName(const std::string& rstrName, long rlValue);
		bool UpdateValueByName(const std::string& rstrName, float rfValue);
		bool UpdateValueByName(const std::string& rstrName, double rdValue);
		bool UpdateValueByName(const std::string& rstrName, const CJsonData& rDataCon);

		/*更新数组里面的内容update value by index*/
		bool UpdateValueByIndex( uint32_t ruIndex, const std::string& rstrValue);	/*值为字符串*/
		bool UpdateValueByIndex( uint32_t ruIndex, const char* pValue);			/*值为字符串*/
		bool UpdateValueByIndex( uint32_t ruIndex, bool rbValue);			/*值为布尔类型（bool）*/
		bool UpdateValueByIndex( uint32_t ruIndex, int32_t riValue);		/*值为int*/
		bool UpdateValueByIndex( uint32_t ruIndex, long rlValue);
		bool UpdateValueByIndex( uint32_t ruIndex, float rfValue);
		bool UpdateValueByIndex( uint32_t ruIndex, double rdValue);
		bool UpdateValueByIndex( uint32_t ruIndex, const CJsonData& rDataCon);

		/*写object*/
		bool PushValue(const std::string& rstrName, const std::string& rstrValue);	/*值为字符串*/
		bool PushValue(const std::string& rstrName, const char* pValue);			/*值为字符串*/
		bool PushValue(const std::string& rstrName, bool rbValue);			/*值为布尔类型（bool）*/
		bool PushValue(const std::string& rstrName, int32_t riValue);		/*值为int*/
		bool PushValue(const std::string& rstrName, long rlValue);
		bool PushValue(const std::string& rstrName, float rfValue);
		bool PushValue(const std::string& rstrName, double rdValue);
		bool PushValue(const std::string& rstrName, const CJsonData& rDataCon);

		/*写array*/
		bool PushValue(const std::string& rstrValue);	/*值为字符串*/
		bool PushValue(const char* pValue);			/*值为字符串*/
		bool PushValue(bool rbValue);			/*值为布尔类型（bool）*/
		bool PushValue(int32_t riValue);		/*值为int*/
		bool PushValue(long rlValue);
		bool PushValue(float rfValue);
		bool PushValue(double rdValue);
		bool PushValue(const CJsonData& rDataCon);

		/*get*/
		bool GetValueByName(const std::string& rstrName, std::string& rstrValue);	/*值为字符串*/
		bool GetValueByName(const std::string& rstrName, char*& pValue);			/*值为字符串*/
		bool GetValueByName(const std::string& rstrName, bool& rbValue);			/*值为布尔类型*/
		bool GetValueByName(const std::string& rstrName, int32_t& riValue);		/*值为int*/
		bool GetValueByName(const std::string& rstrName, long& rlValue);
		bool GetValueByName(const std::string& rstrName, float& rfValue);
		bool GetValueByName(const std::string& rstrName, double& rdValue);
		bool GetValueByName(const std::string& rstrName, CJsonData& rDataCon);
		/*针对数组类型的*/
		bool GetValueByIndex(uint32_t ruIndex, std::string& rstrValue);	/*数组值为字符串*/
		bool GetValueByIndex(uint32_t ruIndex, int32_t& riValue);			/*数组值为int*/
		bool GetValueByIndex(uint32_t ruIndex, float& rfValue);			/*数组值为float*/
		bool GetValueByIndex(uint32_t ruIndex, long& rlValue);				/*数组值为long*/
		bool GetValueByIndex(uint32_t ruIndex, double& rdValue);			/*数组值为double*/
		bool GetValueByIndex(uint32_t ruIndex, CJsonData& rData);		/*数组的值为CJsonData对象*/

		/* 对象迭代 */
		ObjectIterator ObjectBegin();
		ObjectIterator ObjectEnd();

		ConstObjectIterator ObjectBegin() const;
		ConstObjectIterator ObjectEnd() const;

		/* 数组迭代器 */
		ArrayIterator ArrayBegin();
		ArrayIterator ArrayEnd();

		ConstArrayIterator ArrayBegin() const;
		ConstArrayIterator ArrayEnd() const;

		/*删除容器中指定名称的数据*/
		bool RemoveValue(const std::string& rstrName);

		/*如果是数组，则返回该数组的数量*/
		/*判断是否包含子项rstrName*/
		bool IsHasMember(const std::string& rstrName);

		uint32_t GetSize();

		std::shared_ptr<rapidjson::Value::AllocatorType>& Allocator() { return mAlloc; }
		std::shared_ptr<rapidjson::Value>& Value() { return mValue; }
		
	protected:

		void Init();
		bool CheckPush(const std::string &rstrName);

		std::shared_ptr<rapidjson::Value::AllocatorType> mAlloc;
		std::shared_ptr<rapidjson::Value> mValue;
	};
}
#endif
