#include <iostream>
#include <vector>
#include <ctime>
using namespace std;

string currentDateTime() {
	time_t t = time(nullptr);
	tm now;
	errno_t is_error = localtime_s(&now, &t);

	if (is_error == 0) {
		char buffer[256];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &now);
		return buffer;
	}
	else {
		return "���� �ð��� ���� �� ����";
	}
}

enum class _error : int { shut_down, ValueErrorInt, ValueErrorChar, UnknownError };

// C++ ���� �޽��� ���� : https://learn.microsoft.com/ko-kr/cpp/error-messages/compiler-errors-1/c-cpp-build-errors?view=msvc-170
void error(_error code, string message = "") {
	if (!message.empty())
		cout << "error: " << message << "\n";

	switch (code) {
	case _error::shut_down:
		cout << "���α׷� ������ ����\n";
		break;
	case _error::ValueErrorInt: // �߸��� �Է� - int
		cout << "ValueErrorInt: int ���� �ԷµǾ�� �մϴ�.\n";
		break;
	case _error::ValueErrorChar: // �߸��� �Է� - char
		cout << "ValueErrorChar: char ���� �ԷµǾ�� �մϴ�.\n";
		break;
	default:
		cout << "UnknownError: �� �� ���� ����\n";
	}

	exit(1); // ���α׷� ������ ����
}

struct Edge {
	int from; // ������
	int to; // ����
	int w; // ����ġ
};

class Graph {
private:
	unsigned v; // ���� ��
	vector<Edge> edges; // �׷����� ���� ������

public:
	// ������
	Graph() { this->v = 0; }
	Graph(unsigned v) { this->v = v; }

	// �Լ� ���ǿ� ���� const : �� �Լ� �ȿ��� ���� ������ ������ �� ����
	unsigned size() const { return v; } // �׷����� ���� ������ ���� ��ȯ
	auto& edges_from() const { return this->edges; } // �׷����� ���� �������� ��ȯ
	// Ư�� ������ ����� �����鸸 ��ȯ
	auto edges_from(unsigned i) const { // ���⿡ & ���� ����� ����� ��ȯ�� �� ��. �ٵ� ������ ��..
		vector<Edge> edge_from_i;
		for (auto& e : edges) {
			if (e.from == i)
				edge_from_i.push_back(e);
		}
		/* // ���� �ڵ嵵 �Ȱ��� ����� ��
		for (int idx = 0; idx < this->edges.size(); idx++) {
			if (this->edges[idx].from == i)
				edge_from_i.push_back(edges[idx]);
		}*/
		return edge_from_i;
	}

	void add(Edge&& e) { // ���� ���� �߰�
		if (e.from > 0 && e.from <= this->v && e.to > 0 && e.to <= this->v)
			this->edges.push_back(e);
		else
			error(_error::shut_down, "���� ���� �ʰ�");

		return;
	}

	void add_undir(Edge&& e) { // ������ ���� �߰�
		if (e.from > 0 && e.from <= this->v && e.to > 0 && e.to <= this->v) {
			this->edges.push_back(e);
			this->edges.push_back(Edge{e.to, e.from, e.w});
		}
		else
			error(_error::shut_down, "���� ���� �ʰ�");

		return;
	}

	void print() { // �׷��� ���
		for (int i = 1; i <= v; i++) {
			cout << "# " << i << ": "; // ���� ��ȣ
			vector<Edge> edge = this->Graph::edges_from(i); // ������ ����� ���� ��������
			for (auto& e : edge)
				cout << "(" << e.to << ", " << e.w << ")  "; // ������ ����� ���� ���
			cout << "\n";
		}
		return;
	}
};

class GA {
private:
	Graph graph;
	vector<string> pool;
public:
	GA() { graph = Graph(); }
};

int main()
{
	// ���� �����
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	// �ڵ� ���� �ð� ���� : https://scarlettb.tistory.com/5
	clock_t clock_start, clock_finish;
	double clock_duration;

	cout << "main.cpp ���� �ð� : " << currentDateTime() << "\n\n";

	clock_start = clock();

	int v, e; // ���� �� v, ���� �� e
	int from, to, w; // �����, ������, ����ġ
	Graph graph;

	cin >> v >> e;

	graph = Graph(v);

	for (int i = 0; i < e; i++) {
		cin >> from >> to >> w;
		graph.add_undir(Edge{ from, to, w });
	}

	graph.print();

	clock_finish = clock();

	// clock_duration = (double)(clock_finish - clock_start) / CLOCKS_PER_SEC; // �� ������ ȯ��
	clock_duration = (double)(clock_finish - clock_start);
	cout << "\n���α׷� ���� �ð� : " << clock_duration << "ms\n";
	
	return 0;
}