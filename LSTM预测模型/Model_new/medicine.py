import numpy as np
import pandas as pd
from tensorflow.keras.models import Sequential, load_model
from tensorflow.keras.layers import LSTM, Dense, Dropout
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, confusion_matrix, classification_report
import joblib
import matplotlib.pyplot as plt
import seaborn as sns
from datetime import datetime
import os

# 设置中文字体支持
plt.rcParams['font.sans-serif'] = ['SimHei']  # 用来正常显示中文标签
plt.rcParams['axes.unicode_minus'] = False  # 用来正常显示负号

# ======================
# 数据预处理
# ======================
def preprocess_data(file_path, scheduled_times):
    """
    预处理服药记录数据
    :param file_path: 服药记录文件路径
    :param scheduled_times: 预设服药时间列表，如 ['09:30', '14:20', '20:30']
    :return: 特征矩阵X, 标签向量y
    """
    # 1. 读取数据
    try:
        df = pd.read_csv(file_path, sep=' ', header=None, 
                        names=['date', 'time', 'user'], 
                        parse_dates={'datetime': [0, 1]})
    except Exception as e:
        print(f"文件读取错误: {e}")
        return None, None
    
    # 2. 确保数据格式正确
    if 'datetime' not in df.columns:
        print("错误: 数据格式不正确，缺少'datetime'列")
        return None, None
        
    # 3. 创建完整日期序列（每天一个数据点）
    start_date = df['datetime'].min().normalize()  # 获取最早日期
    end_date = df['datetime'].max().normalize()    # 获取最晚日期
    date_range = pd.date_range(start=start_date, end=end_date, freq='D')
    
    # 4. 构建每日服药状态特征
    features = []
    labels = []  # 第二天是否漏服
    
    # 将预设时间转换为分钟数
    scheduled_minutes = [int(t[:2])*60 + int(t[3:]) for t in scheduled_times]
    
    for i in range(len(date_range)-1):  # 最后一天无法预测第二天
        current_day = date_range[i]
        next_day = date_range[i+1]
        
        # 4.1 获取当天所有服药记录
        day_records = df[(df['datetime'] >= current_day) & 
                        (df['datetime'] < current_day + pd.Timedelta(days=1))]
        
        # 4.2 构建特征向量
        day_features = []
        delays = []  # 用于计算平均延迟
        
        # 特征1：当天是否完成每次服药
        for time_min in scheduled_minutes:
            scheduled_time = current_day + pd.Timedelta(minutes=time_min)
            # 检查±30分钟内是否有服药记录
            taken_records = day_records[
                (day_records['datetime'] >= scheduled_time - pd.Timedelta(minutes=30)) &
                (day_records['datetime'] <= scheduled_time + pd.Timedelta(minutes=30))
            ]
            taken = 1 if len(taken_records) > 0 else 0
            day_features.append(taken)
            
            # 计算延迟
            if taken and not taken_records.empty:
                closest = taken_records.iloc[0]
                delay = (closest['datetime'] - scheduled_time).total_seconds() / 60
                delays.append(delay)
        
        # 特征2：当天服药的平均延迟时间（分钟）
        avg_delay = np.mean(delays) if delays else 0
        day_features.append(avg_delay)
        
        # 特征3：星期几（0=周一，6=周日）
        day_features.append(current_day.dayofweek)
        
        features.append(day_features)
        
        # 4.3 构建标签：第二天是否漏服至少一次
        next_day_records = df[(df['datetime'] >= next_day) & 
                             (df['datetime'] < next_day + pd.Timedelta(days=1))]
        
        next_day_missed = False
        for time_min in scheduled_minutes:
            scheduled_time = next_day + pd.Timedelta(minutes=time_min)
            taken = any(
                (next_day_records['datetime'] >= scheduled_time - pd.Timedelta(minutes=30)) &
                (next_day_records['datetime'] <= scheduled_time + pd.Timedelta(minutes=30))
            )
            if not taken:
                next_day_missed = True
                break
        
        labels.append(1 if next_day_missed else 0)  # 1=漏服，0=无漏服
    
    return np.array(features), np.array(labels)

# ======================
# LSTM模型构建
# ======================
def build_lstm_model(input_shape):
    model = Sequential([
        LSTM(32, input_shape=input_shape, return_sequences=False),
        Dropout(0.3),
        Dense(16, activation='relu'),
        Dense(1, activation='sigmoid')  # 输出漏服概率
    ])
    
    model.compile(optimizer='adam',
                  loss='binary_crossentropy',
                  metrics=['accuracy'])
    
    model.summary()
    return model

# ======================
# 预测函数
# ======================
def predict_risk(model, scaler, yesterday_features):
    """
    预测今日漏服风险
    :param model: 训练好的LSTM模型
    :param scaler: 数据标准化器
    :param yesterday_features: 昨天的服药状态特征
    :return: 今日漏服风险概率 (0.0-1.0)
    """
    # 确保输入格式正确
    if len(yesterday_features) != model.input_shape[2]:
        print(f"错误: 输入特征数应为{model.input_shape[2]}, 实际为{len(yesterday_features)}")
        return None
    
    # 转换为NumPy数组
    features_array = np.array([yesterday_features])
    
    # 标准化
    features_scaled = scaler.transform(features_array)
    
    # 重塑为LSTM输入格式 [样本数, 时间步长=1, 特征数]
    input_data = features_scaled.reshape(1, 1, -1)
    
    # 预测
    risk = model.predict(input_data)[0][0]
    return risk

# ======================
# 可视化函数
# ======================
def plot_training_history(history, save_path=None):
    """
    绘制训练过程中的损失和准确率曲线
    """
    plt.figure(figsize=(12, 5))
    
    # 绘制损失曲线
    plt.subplot(1, 2, 1)
    plt.plot(history.history['loss'], label='训练损失')
    plt.plot(history.history['val_loss'], label='验证损失')
    plt.title('训练和验证损失曲线')
    plt.ylabel('损失')
    plt.xlabel('迭代次数')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    
    # 绘制准确率曲线
    plt.subplot(1, 2, 2)
    plt.plot(history.history['accuracy'], label='训练准确率')
    plt.plot(history.history['val_accuracy'], label='验证准确率')
    plt.title('训练和验证准确率曲线')
    plt.ylabel('准确率')
    plt.xlabel('迭代次数')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(f"{save_path}/training_history.png", dpi=300, bbox_inches='tight')
    plt.show()

def plot_confusion_matrix(y_true, y_pred, classes, save_path=None):
    """
    绘制混淆矩阵
    """
    cm = confusion_matrix(y_true, y_pred)
    plt.figure(figsize=(8, 6))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
                xticklabels=classes, yticklabels=classes)
    plt.title('漏服预测混淆矩阵')
    plt.ylabel('实际标签')
    plt.xlabel('预测标签')
    
    if save_path:
        plt.savefig(f"{save_path}/confusion_matrix.png", dpi=300, bbox_inches='tight')
    plt.show()

def plot_risk_distribution(risks, labels, save_path=None):
    """
    绘制漏服风险分布图
    """
    plt.figure(figsize=(10, 6))
    
    # 实际漏服日的风险分布
    plt.hist([r for r, l in zip(risks, labels) if l == 1], 
             bins=20, alpha=0.7, label='实际漏服日')
    
    # 实际未漏服日的风险分布
    plt.hist([r for r, l in zip(risks, labels) if l == 0], 
             bins=20, alpha=0.7, label='实际未漏服日')
    
    plt.title('漏服风险概率分布')
    plt.xlabel('预测风险概率')
    plt.ylabel('天数')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    
    if save_path:
        plt.savefig(f"{save_path}/risk_distribution.png", dpi=300, bbox_inches='tight')
    plt.show()

def plot_feature_importance(model, feature_names, save_path=None):
    """
    可视化特征重要性
    """
    # 获取第一层LSTM的权重
    lstm_layer = model.layers[0]
    weights = lstm_layer.get_weights()[0]  # 输入权重
    
    # 计算特征重要性（平均绝对值）
    importance = np.mean(np.abs(weights), axis=1)
    
    # 只取与特征数量匹配的部分
    importance = importance[:len(feature_names)]
    
    # 排序
    sorted_idx = np.argsort(importance)
    
    plt.figure(figsize=(10, 6))
    plt.barh(range(len(sorted_idx)), importance[sorted_idx], align='center')
    plt.yticks(range(len(sorted_idx)), [feature_names[i] for i in sorted_idx])
    plt.title('特征重要性分析 (基于LSTM输入权重)')
    plt.xlabel('平均权重绝对值')
    plt.grid(True, linestyle='--', alpha=0.3, axis='x')
    plt.tight_layout()
    
    if save_path:
        plt.savefig(f"{save_path}/feature_importance.png", dpi=300, bbox_inches='tight')
    plt.show()

def plot_risk_over_time(model, X_test, y_test, save_path=None):
    """
    绘制风险随时间变化的曲线
    """
    # 获取预测概率
    y_pred_proba = model.predict(X_test).flatten()
    
    # 排序
    sorted_idx = np.argsort(y_pred_proba)
    sorted_risk = y_pred_proba[sorted_idx]
    sorted_labels = y_test[sorted_idx]
    
    plt.figure(figsize=(12, 6))
    
    # 绘制风险曲线
    plt.plot(sorted_risk, label='预测风险', color='blue', linewidth=2)
    
    # 标记实际漏服点
    missed_indices = np.where(sorted_labels == 1)[0]
    plt.scatter(missed_indices, sorted_risk[missed_indices], 
                color='red', s=50, zorder=3, label='实际漏服')
    
    # 添加阈值线
    plt.axhline(y=0.5, color='orange', linestyle='--', label='风险阈值 (0.5)')
    
    plt.title('测试集风险预测分布')
    plt.xlabel('样本索引 (按风险排序)')
    plt.ylabel('预测漏服风险')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    
    if save_path:
        plt.savefig(f"{save_path}/risk_over_time.png", dpi=300, bbox_inches='tight')
    plt.show()

# ======================
# 主程序
# ======================
def train_and_save_model(data_path, scheduled_times, model_save_path="model"):
    """
    训练并保存模型
    :param data_path: 服药记录文件路径
    :param scheduled_times: 预设服药时间列表
    :param model_save_path: 模型保存路径
    """
    # 确保目录存在
    os.makedirs(model_save_path, exist_ok=True)
    
    # 1. 预处理数据
    print("正在预处理数据...")
    X, y = preprocess_data(data_path, scheduled_times)
    
    if X is None or y is None:
        print("数据预处理失败，请检查输入文件")
        return
    
    print(f"数据预处理完成，共 {len(X)} 个样本")
    print(f"漏服率: {sum(y)/len(y):.2%}")
    
    # 2. 标准化特征
    scaler = MinMaxScaler()
    X_scaled = scaler.fit_transform(X)
    
    # 3. 重塑为LSTM输入格式 [样本数, 时间步长=1, 特征数]
    X_reshaped = X_scaled.reshape((X_scaled.shape[0], 1, X_scaled.shape[1]))
    
    # 4. 划分数据集
    X_train, X_test, y_train, y_test = train_test_split(
        X_reshaped, y, test_size=0.2, random_state=42, stratify=y
    )
    
    # 5. 构建并训练模型
    print("构建LSTM模型...")
    model = build_lstm_model((X_train.shape[1], X_train.shape[2]))
    
    print("开始训练模型...")
    history = model.fit(X_train, y_train, 
                        epochs=100, 
                        batch_size=8, 
                        validation_split=0.1,
                        verbose=1)
    
    # 6. 可视化训练过程
    print("绘制训练过程...")
    plot_training_history(history, model_save_path)
    
    # 7. 评估模型
    print("评估模型性能...")
    # 获取测试集预测概率
    y_pred_proba = model.predict(X_test)
    y_pred = (y_pred_proba > 0.5).astype(int)
    
    # 计算评估指标
    test_acc = accuracy_score(y_test, y_pred)
    print(f"测试准确率: {test_acc:.2%}")
    print("\n分类报告:")
    print(classification_report(y_test, y_pred, target_names=['未漏服', '漏服']))
    
    # 绘制混淆矩阵
    plot_confusion_matrix(y_test, y_pred, classes=['未漏服', '漏服'], save_path=model_save_path)
    
    # 绘制风险分布
    plot_risk_distribution(y_pred_proba, y_test, save_path=model_save_path)
    
    # 特征名称
    time_names = [f"{t}服药" for t in scheduled_times]
    feature_names = time_names + ['平均延迟(分钟)', '星期几']
    
    # 绘制特征重要性
    plot_feature_importance(model, feature_names, save_path=model_save_path)
    
    # 绘制风险随时间变化
    plot_risk_over_time(model, X_test, y_test, save_path=model_save_path)
    
    # 8. 保存模型
    print("保存模型和参数...")
    # 保存模型
    model.save(f"{model_save_path}/medication_risk_model.h5")
    
    # 保存标准化器
    joblib.dump(scaler, f"{model_save_path}/scaler.pkl")
    
    # 保存特征信息
    feature_info = {
        "scheduled_times": scheduled_times,
        "num_times": len(scheduled_times),
        "input_shape": (1, X_train.shape[2])
    }
    joblib.dump(feature_info, f"{model_save_path}/feature_info.pkl")
    
    print(f"模型已保存至: {model_save_path}/")
    
    # 返回模型和测试数据用于进一步分析
    return model, X_test, y_test

# ======================
# 加载模型进行预测
# ======================
def load_and_predict(model_path, yesterday_features):
    """
    加载模型并进行预测
    :param model_path: 模型保存路径
    :param yesterday_features: 昨天的服药状态特征
    :return: 今日漏服风险概率
    """
    # 加载模型
    model = load_model(f"{model_path}/medication_risk_model.h5")
    scaler = joblib.load(f"{model_path}/scaler.pkl")
    feature_info = joblib.load(f"{model_path}/feature_info.pkl")
    features = feature_info["num_times"] + 2  # 服药次数 + 平均延迟 + 星期几
    if len(yesterday_features) != features:
        print(f"错误: 需要 {features} 个特征，实际提供 {len(yesterday_features)} 个")
        return None
    # 预测风险
    risk = predict_risk(model, scaler, yesterday_features)
    return risk

if __name__ == "__main__":

    DATA_PATH = "D:/Code/VSCode_Project/Python/Model_new/medication_records.txt"
    SCHEDULED_TIMES = ['09:30', '14:20', '20:30']  # 预设服药时间
    
    # 训练并保存模型
    model, X_test, y_test = train_and_save_model(DATA_PATH, SCHEDULED_TIMES, "risk_model")

    # 昨天服药情况特征: [早, 中, 晚, 平均延迟, 星期几]
    # 示例: 昨天早、中按时服药，晚上漏服，平均延迟15分钟，昨天是周三(2)
    # yesterday_data = [1, 1, 1, 0, 1]
    
    # # 预测今日风险
    # risk = load_and_predict("D:/Code/VSCode_Project/Python/risk_model", yesterday_data)
    
    # if risk is not None:
    #     print(f"今日漏服风险概率: {risk:.2%}")
    #     if risk > 0.7:
    #         print("警告: 高风险! 加强提醒")
    #     elif risk > 0.4:
    #         print("注意: 中等风险! 保持提醒")
    #     else:
    #         print("低风险! 正常提醒即可")