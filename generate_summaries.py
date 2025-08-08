import json
from app_trai_trao import trai_summarize, trao_summarize

# 输入文件名
input_file = "nlpcc_data.json"
# 输出文件名
output_file = "summarized_output.jsonl"

def process_json_data(data_list):
    """处理JSON数据列表"""
    processed_count = 0
    
    with open(output_file, "w", encoding="utf-8") as outfile:
        for item in data_list:
            if isinstance(item, dict):
                content = item.get("content", "").strip()
                title = item.get("title", "")

                if content:
                    try:
                        # 调用 TRAI/TRAO 摘要模型
                        trai_summary = trai_summarize(content)
                        trao_summary = trao_summarize(content)

                        result = {
                            "title": title,
                            "trai": trai_summary,
                            "trao": trao_summary
                        }

                        # 写入摘要结果
                        outfile.write(json.dumps(result, ensure_ascii=False) + "\n")
                        processed_count += 1
                        
                    except Exception as e:
                        print(f"处理摘要时出错 (title: {title}): {e}")
                        continue
                else:
                    print(f"跳过空内容项 (title: {title})")
            else:
                print(f"跳过非字典类型的数据项: {type(item)}")
    
    return processed_count

def main():
    try:
        with open(input_file, "r", encoding="utf-8") as infile:
            file_content = infile.read().strip()
            
            if not file_content:
                print(f"错误：输入文件 {input_file} 为空")
                return
            
            # 尝试解析为JSONL格式（每行一个JSON对象）
            if file_content.count('\n') > 0:
                try:
                    data_list = []
                    for line_num, line in enumerate(file_content.split('\n'), 1):
                        line = line.strip()
                        if line:  # 跳过空行
                            try:
                                data = json.loads(line)
                                data_list.append(data)
                            except json.JSONDecodeError as e:
                                print(f"警告：第{line_num}行JSON解析失败: {e}")
                                # 如果JSONL格式失败，尝试整个文件作为JSON数组
                                break
                    else:
                        # 所有行都成功解析为JSONL
                        if data_list:
                            print(f"检测到JSONL格式，共{len(data_list)}条记录")
                            processed = process_json_data(data_list)
                            print(f"批量摘要完成，处理了{processed}条记录，结果已保存至：{output_file}")
                            return
                except Exception as e:
                    print(f"JSONL格式解析失败: {e}")
            
            # 尝试解析为单个JSON数组
            try:
                data = json.loads(file_content)
                if isinstance(data, list):
                    print(f"检测到JSON数组格式，共{len(data)}条记录")
                    processed = process_json_data(data)
                    print(f"批量摘要完成，处理了{processed}条记录，结果已保存至：{output_file}")
                elif isinstance(data, dict):
                    print("检测到单个JSON对象格式")
                    processed = process_json_data([data])
                    print(f"批量摘要完成，处理了{processed}条记录，结果已保存至：{output_file}")
                else:
                    print(f"错误：不支持的JSON数据类型: {type(data)}")
            except json.JSONDecodeError as e:
                print(f"JSON解析失败: {e}")
                print("请检查输入文件格式是否正确")
                print("支持的格式:")
                print("1. JSONL格式（每行一个JSON对象）")
                print("2. JSON数组格式")
                print("3. 单个JSON对象格式")
                
    except FileNotFoundError:
        print(f"错误：找不到输入文件 {input_file}")
    except Exception as e:
        print(f"处理文件时出错: {e}")

if __name__ == "__main__":
    main()