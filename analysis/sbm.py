import networkx as nx
import matplotlib.pyplot as plt

G = nx.DiGraph()
G.add_edge(1,2)
nx.draw(G)
plt.show()