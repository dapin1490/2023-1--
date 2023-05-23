#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <ctime>
#include <limits>
using namespace std;


/***************

<����>
1. �׷��� �Է�
2. ������ �� ����
3. ����ġ ���
4. �θ�Ǯ ����
5. Selection
6. Crossover
7. �ڽ�Ǯ ����
8. Replace
9. Ǯ ����ġ ����, ���� ��, ��� ���
10. main

*****************/





//�׷��� ������ ���� ����ü ����
struct Edge {
    int node1;
    int node2;
    int weight;
};


//�׷��� �б�
vector<Edge> readGraphFromFile(const string& filename) {
    vector<Edge> graph;
    ifstream file(filename);
    if (file.is_open()) {
        int numVertices, numEdges;
        file >> numVertices >> numEdges; //ù��

        for (int i = 0; i < numEdges; i++) {
            Edge edge;
            file >> edge.node1 >> edge.node2 >> edge.weight;
            graph.push_back(edge);
        }

        file.close();
        cout << "���� ����: " << filename << "." << endl;
    }
    else {
        cerr << filename <<"������ ���� �� �����ϴ�." << endl;
    }

    return graph;
}


//������ �� ����
string generateDNA(const vector<Edge>& graph) {
    string dna;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for (const auto& edge : graph) {
        if (dis(gen) <= 0.5) {
            dna += "1 ";
        }
        else {
            dna += "0 ";
        }
    }
    return dna;
}


//����ġ ���
int calculateWeight(const vector<Edge>& graph, const string& chromosome) {
    int Weight = 0;
    for (const auto& edge : graph) {
        if (chromosome[edge.node1 - 1] == '1' && chromosome[edge.node2 - 1] == '0') {
            Weight += edge.weight;
        }
        else if (chromosome[edge.node1 - 1] == '0' && chromosome[edge.node2 - 1] == '1') {
            Weight += edge.weight;
        }
    }
    return Weight;
}


 
//�θ�Ǯ ����
vector<pair<string, double>> parentPool;

void genParentPool(const vector<Edge>& graph) {
    int population=graph.size()*10;
    
    if (population > 1000) {
        population = 1000;
    }

    for (int i = 1; i <= population; i++) {
        string chromo = generateDNA(graph);
        //����ġ ����ؼ� Weight�� ����
        int Weight = calculateWeight(graph, chromo);
        parentPool.emplace_back(chromo, Weight); //�θ�Ǯ�� ����
    }
}



// �����ڿ� ����ġ�� �����ϴ� ��� Ÿ�� ����
using GeneWeightPair_Parent = pair<string, double>;

// Selection �Լ� ����
pair<GeneWeightPair_Parent, GeneWeightPair_Parent> Selection(const vector<GeneWeightPair_Parent>& parentPool) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, parentPool.size() - 1);

    while (true) {
        // �����ϰ� �� ���� ������ ����
        int index1 = dist(gen);
        int index2 = dist(gen);

        const GeneWeightPair_Parent& gene1 = parentPool[index1];
        const GeneWeightPair_Parent& gene2 = parentPool[index2];

        // �� �������� ����ġ ���� ���
        double weightDiff = abs(gene1.second - gene2.second);
        
        // maxDiff�� �θ�Ǯ ����ġ �ִ�, �ּҰ��� ���� 50%�� �� ����
        double maxDiff = (max_element(parentPool.begin(), parentPool.end(),
            [](const GeneWeightPair_Parent& a, const GeneWeightPair_Parent& b) {
                return a.second < b.second;
            })->second) * 0.5;

        if (weightDiff <= maxDiff) {
            // ������ �����ϴ� ��� ������ �����ڵ� ��ȯ
            return make_pair(gene1, gene2);
        }
    }
}



//Crossover �ؼ� child�� ��ȯ�ϴ� ������ 
string Crossover(const string& gene1, const string& gene2) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    string child;
    int count = 1;

    for (size_t i = 0; i < gene1.length(); ++i) {
        // ����ġ�� �� ���� �������� 60%�� �״�� ��������
        if (dis(gen) <= 0.6) {
            child += gene1[i];
        }
        else {
            // ������ 40%�� �ٸ� �����ڿ��� ��������
            child += gene2[i];
        }
        // �������� �����ϱ�
        if (dis(gen) <= (0.0005 * count)) {
            // ������ �ڸ� ����
            if (child[i] == '0') {
                child[i] = '1';
            }
            else {
                child[i] = '0';
            }
        }

        // ��� ������ 10�� ���� ������ count ����
        if ((i + 1) % 10 == 0) {
            count++;
        }
    }return child;
    
}



//�ڽ�Ǯ ���� �Լ�
vector<pair<string, double>> childPool;
void genchildPool(const vector<Edge>& graph) {
    //������: �θ�Ǯ�� ����
    int parentSize = parentPool.size();
    
    for (int i = 1; i <= parentSize; i++) {
        pair<GeneWeightPair_Parent, GeneWeightPair_Parent> selectedGenes = Selection(parentPool); //selection
        string child = Crossover(selectedGenes.first.first, selectedGenes.second.first); //crossover
        string chromo = child;
        int Weight = calculateWeight(graph, chromo); //����ġ ���
        childPool.emplace_back(chromo, Weight); //�ڽ�Ǯ�� pair ����
    }
}


// replace: childPool�� parentPool�� ��ü�ϰ� parentPool�� �� ���ͷ� ����� �Լ�
void replace(vector<int>& parentPool, vector<int>& childPool) {
    // parentPool�� �� ����ü�� ���� ����ġ�� ����ϰ� �ջ� �� �ִ밪 ������Ʈ
    
    parentPool = childPool;  // parentPool�� childPool�� ��ü
    childPool.clear();  // childPool�� �� ���ͷ� �ʱ�ȭ
    
}


//Ǯ ����ġ ����, ���� ��, ��� ���
void calculateParentPoolStats(const vector<Edge>& graph, const vector<pair<string, double>>& parentPool) {
    double totalWeight = 0.0; // ����ġ ����
    double maxWeight = numeric_limits<double>::min(); // �ִ� ����ġ
    pair<string, double> maxWeightPair; // �ִ� ����ġ�� ���� ��
    double sumSquaredDiff = 0.0; // �������� ��

    for (const auto& pair : parentPool) {
        const string& chromosome = pair.first;
        double weight = calculateWeight(graph, chromosome);
        totalWeight += weight;

        if (weight > maxWeight) {
            maxWeight = weight;
            maxWeightPair = pair;
        }
        double diff = weight - totalWeight / parentPool.size();
        sumSquaredDiff += diff * diff;
    }
    double averageWeight = totalWeight / parentPool.size(); // ����ġ ���
    double variance = sumSquaredDiff / parentPool.size(); // �л�
    double standardDeviation = std::sqrt(variance); // ǥ������


    cout << "�ִ� ����ġ: " << maxWeight << endl;
    cout << "�θ�Ǯ ����ġ ����: " << totalWeight << endl;
    cout << "����ġ ���: " << averageWeight << endl;
    cout << "ǥ������: " << standardDeviation << endl;
    cout << "�ִ� ����ġ�� ���� ��: " << maxWeightPair.first << endl;
}


int main() {

    string inputFilename = "unweighted_50.txt";  // �ش� ���� �б�
    vector<Edge> graph = readGraphFromFile(inputFilename);
    genParentPool(graph);  //�ʱ� �θ�Ǯ ����

    //���� �ð� ����
    int seconds = 10;
    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();
    parentPool = parentPool;
    childPool = childPool;


    //���� �ð����� ����
    while (true) {
        chrono::time_point<chrono::system_clock> currentTime = chrono::system_clock::now();
        chrono::duration<double> elapsedSeconds = currentTime - startTime;

        //���� �ð� ���޽� ����
        if (elapsedSeconds.count() >= seconds) {
            break;
        }

        //���뱳ü �ݺ� 
        while (true) {
            genchildPool(graph); // ����, ����, �ݺ��� ���� �ڽ�Ǯ ����
            //replace(parentPool, childPool); // ���� ��ü
            break;
        }   
    }
    
    calculateParentPoolStats(graph, parentPool);
    return 0;
}
