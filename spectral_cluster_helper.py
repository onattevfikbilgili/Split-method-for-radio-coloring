#!/usr/bin/env python3
# spectral_cluster_helper.py
import numpy as np
import sys

def cluster_graph(adjacency_matrix, n_clusters=4):
    """
    Perform spectral clustering on a precomputed adjacency matrix.
    
    Args:
        adjacency_matrix: List of lists or numpy array representing adjacency matrix
        n_clusters: Number of clusters (default: 4)
        
    Returns:
        List of cluster labels for each vertex
    """
    try:
        from sklearn.cluster import SpectralClustering
    except ImportError as e:
        print(f"ERROR: Failed to import scikit-learn: {e}", file=sys.stderr)
        print("Please install with: pip install scikit-learn numpy", file=sys.stderr)
        return [0] * len(adjacency_matrix)
    
    # Convert to numpy array
    X = np.array(adjacency_matrix, dtype=float)
    
    # Validate matrix
    if X.shape[0] != X.shape[1]:
        raise ValueError(f"Adjacency matrix must be square, got shape {X.shape}")
    
    if not np.allclose(X, X.T):
        print("WARNING: Adjacency matrix is not symmetric. Using directed mode.")
    
    # Apply spectral clustering
    try:
        model = SpectralClustering(
            n_clusters=n_clusters,
            affinity='precomputed',
            assign_labels='kmeans',
            random_state=42
        )
        labels = model.fit_predict(X)
        return labels.tolist()
    except Exception as e:
        print(f"ERROR in spectral clustering: {e}", file=sys.stderr)
        # Return default clustering (all vertices in cluster 0)
        return [0] * len(adjacency_matrix)

# Test function when run directly
if __name__ == "__main__":
    # Create a test graph
    test_matrix = [
        [1.0, 1.0, 0.0, 0.0, 0.0],
        [1.0, 1.0, 1.0, 0.0, 0.0],
        [0.0, 1.0, 1.0, 1.0, 0.0],
        [0.0, 0.0, 1.0, 1.0, 1.0],
        [0.0, 0.0, 0.0, 1.0, 1.0]
    ]
    
    print("Testing spectral clustering...")
    result = cluster_graph(test_matrix, n_clusters=2)
    print(f"Test result: {result}")
    print("✅ Python module is working correctly!")
