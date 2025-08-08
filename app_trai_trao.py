import re
import jieba
import numpy as np
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity
import networkx as nx
import torch
from transformers import BertModel, BertTokenizer

# Load BERT model and tokenizer for Chinese text
tokenizer = BertTokenizer.from_pretrained('bert-base-chinese')
model = BertModel.from_pretrained('bert-base-chinese')

def preprocess_sentences(text):
    """Preprocess text into sentences by removing whitespace and splitting on punctuation."""
    text = re.sub(r'\s+', '', text)
    sentences = re.split(r'[。！？!?]', text)
    return [s.strip() for s in sentences if s.strip()]

def get_sentence_embeddings(sentences):
    """Get BERT embeddings for sentences using self-attention mechanism."""
    if not sentences:
        return np.array([])
    
    # Tokenize sentences for BERT
    inputs = tokenizer(
        sentences, 
        padding=True, 
        truncation=True, 
        max_length=512, 
        return_tensors='pt'
    )
    
    # Generate embeddings using BERT
    with torch.no_grad():
        outputs = model(**inputs)
    
    # Use [CLS] token embeddings as sentence representations
    embeddings = outputs.last_hidden_state[:, 0, :].cpu().numpy()
    return embeddings

def get_tfidf_similarity(sentences):
    """Calculate TF-IDF based similarity matrix for sentences."""
    if len(sentences) < 2:
        return np.array([[1.0]])
    
    sentence_words = [' '.join(jieba.cut(s)) for s in sentences]
    tfidf = TfidfVectorizer()
    try:
        matrix = tfidf.fit_transform(sentence_words)
        sim_matrix = cosine_similarity(matrix)
        return sim_matrix
    except ValueError:
        # Handle case where all sentences are identical or empty
        return np.ones((len(sentences), len(sentences)))

def get_attention_similarity(sentences):
    """Calculate attention-based similarity matrix using BERT embeddings."""
    embeddings = get_sentence_embeddings(sentences)
    if embeddings.size == 0:
        return np.array([[1.0]])
    
    attention_similarity = cosine_similarity(embeddings)
    return attention_similarity

def calculate_textrank_scores(similarity_matrix):
    """Calculate TextRank scores from similarity matrix using PageRank algorithm."""
    if similarity_matrix.shape[0] == 1:
        return {0: 1.0}
    
    # Create graph from similarity matrix
    graph = nx.from_numpy_array(similarity_matrix)
    
    # Calculate PageRank scores
    try:
        scores = nx.pagerank(graph, max_iter=100, tol=1e-6)
        return scores
    except nx.PowerIterationFailedConvergence:
        # Fallback to uniform scores if PageRank fails to converge
        n = similarity_matrix.shape[0]
        return {i: 1.0/n for i in range(n)}

def mmr_selection(sentences, scores, embeddings, summary_size=3, lambda_param=0.7):
    """Select sentences using Maximal Marginal Relevance to reduce redundancy."""
    if embeddings is None or embeddings.size == 0:
        # Fallback to simple ranking if no embeddings available
        ranked = sorted(((scores[i], i, s) for i, s in enumerate(sentences)), reverse=True)
        return [s for _, _, s in ranked[:summary_size]]
    
    selected_indices = []
    selected_sentences = []
    
    for _ in range(min(summary_size, len(sentences))):
        best_score = -float('inf')
        best_idx = -1
        
        for i, sentence in enumerate(sentences):
            if i in selected_indices:
                continue
            
            # Relevance score from TextRank
            score_term = scores.get(i, 0)
            
            if not selected_indices:
                # First sentence selection based purely on TextRank score
                mmr_score = score_term
            else:
                # Calculate similarity to already selected sentences
                candidate_embedding = embeddings[i:i+1]
                summary_embeddings = embeddings[selected_indices]
                
                similarity_to_summary = np.max(cosine_similarity(candidate_embedding, summary_embeddings))
                
                # Calculate MMR score
                mmr_score = lambda_param * score_term - (1 - lambda_param) * similarity_to_summary
            
            if mmr_score > best_score:
                best_score = mmr_score
                best_idx = i
        
        if best_idx != -1:
            selected_indices.append(best_idx)
            selected_sentences.append(sentences[best_idx])
    
    return selected_sentences

def trai_summarize(text, alpha=0.5, summary_size=3):
    """
    TRAI (TextRank with Attention Integration) model.
    Combines TF-IDF and attention-based similarities with weighted fusion.
    """
    sentences = preprocess_sentences(text)
    if len(sentences) <= summary_size:
        return '。'.join(sentences) + '。'
    
    # Get TF-IDF based similarity matrix
    tfidf_sim_matrix = get_tfidf_similarity(sentences)
    
    # Get attention-based similarity matrix
    attn_sim_matrix = get_attention_similarity(sentences)
    
    # Weighted fusion of both similarity matrices
    combined_sim_matrix = alpha * tfidf_sim_matrix + (1 - alpha) * attn_sim_matrix
    
    # Calculate TextRank scores on the combined matrix
    scores = calculate_textrank_scores(combined_sim_matrix)
    
    # Get embeddings for MMR selection
    embeddings = get_sentence_embeddings(sentences)
    
    # Select sentences using MMR to reduce redundancy
    summary_sentences = mmr_selection(sentences, scores, embeddings, summary_size)
    
    return '。'.join(summary_sentences) + '。'

def trao_summarize(text, beta=0.5, summary_size=3):
    """
    TRAO (TextRank with Attention Optimization) model.
    Runs TextRank separately on TF-IDF and attention similarities, then fuses scores.
    """
    sentences = preprocess_sentences(text)
    if len(sentences) <= summary_size:
        return '。'.join(sentences) + '。'
    
    # First TextRank run: TF-IDF based
    tfidf_sim_matrix = get_tfidf_similarity(sentences)
    scores1 = calculate_textrank_scores(tfidf_sim_matrix)
    
    # Second TextRank run: Attention based
    attn_sim_matrix = get_attention_similarity(sentences)
    scores2 = calculate_textrank_scores(attn_sim_matrix)
    
    # Weighted fusion of scores
    final_scores = {}
    for i in range(len(sentences)):
        final_scores[i] = beta * scores1.get(i, 0) + (1 - beta) * scores2.get(i, 0)
    
    # Get embeddings for MMR selection
    embeddings = get_sentence_embeddings(sentences)
    
    # Select sentences using MMR to reduce redundancy
    summary_sentences = mmr_selection(sentences, final_scores, embeddings, summary_size)
    
    return '。'.join(summary_sentences) + '。'

def textrank_graph(sentences):
    """Original TextRank implementation for comparison."""
    sentence_words = [' '.join(jieba.cut(s)) for s in sentences]
    tfidf = TfidfVectorizer()
    matrix = tfidf.fit_transform(sentence_words)
    sim_matrix = cosine_similarity(matrix)
    graph = nx.from_numpy_array(sim_matrix)
    scores = nx.pagerank(graph)
    ranked = sorted(((scores[i], s) for i, s in enumerate(sentences)), reverse=True)
    return [s for _, s in ranked[:3]]

# Example usage and testing
if __name__ == "__main__":
    # Test text
    test_text = """
    人工智能是计算机科学的一个分支，它企图了解智能的实质。人工智能从诞生以来，理论和技术日益成熟。
    应用领域也不断扩大，可以设想，未来人工智能带来的科技产品，将会是人类智慧的容器。
    人工智能可以对人的意识、思维的信息过程的模拟。人工智能不是人的智能，但能像人那样思考。
    也可能超过人的智能。机器学习是人工智能的一个重要分支。深度学习是机器学习的一个子领域。
    神经网络是深度学习的基础。卷积神经网络在图像识别领域表现出色。
    自然语言处理是人工智能的另一个重要应用领域。
    """
    
    print("Original TextRank Summary:")
    sentences = preprocess_sentences(test_text)
    original_summary = '。'.join(textrank_graph(sentences)) + '。'
    print(original_summary)
    print()
    
    print("TRAI Summary (alpha=0.5):")
    trai_result = trai_summarize(test_text, alpha=0.5, summary_size=3)
    print(trai_result)
    print()
    
    print("TRAO Summary (beta=0.5):")
    trao_result = trao_summarize(test_text, beta=0.5, summary_size=3)
    print(trao_result)
    print()
    
    print("TRAI Summary with more attention weight (alpha=0.3):")
    trai_attention_heavy = trai_summarize(test_text, alpha=0.3, summary_size=3)
    print(trai_attention_heavy)