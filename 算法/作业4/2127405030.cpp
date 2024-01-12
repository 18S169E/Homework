#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#define debug(a) cout << "debug : " << (#a)<< " = " << a << endl;

using namespace std;

typedef pair<int, int> pii;
typedef long long ll;
const int N = 1e5 + 10;
const int P = 1e9 + 7;
const int INF = 0x3f3f3f3f;
const double PI = acos(-1.0);//Բ����PI��

struct Complex {
    double x, y;//������x����ʵ����y�����鲿��
    Complex(double _x = 0, double _y = 0) {
        x = _x, y = _y;
    }
}a[N], b[N];//����ʽa��b����ˡ�
int n, m, l, r[N], limit = 1;//nΪa�Ĵ���,mΪb�Ĵ�����limit��Ϊ������ơ�2^n�η�����lΪ�����Ƶ�λ��
//��������ء�
Complex operator + (Complex a, Complex b) {
    return Complex(a.x + b.x, a.y + b.y);
}
Complex operator - (Complex a, Complex b) {
    return Complex(a.x - b.x, a.y - b.y);
}
//������ˣ���ģ����ˣ�������ӡ�
Complex operator * (Complex a, Complex b) {
    return Complex(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}
void fft(Complex* A, int type) {
    for (int i = 0; i < limit; ++i) {
        if (i < r[i])swap(A[i], A[r[i]]);
        //���Ҫ���������䡣С��r[i]ʱ�Ž�������ֹͬһ��Ԫ�ؽ������Σ��ص�ԭ����λ�á�
    }
    //�ӵײ����Ϻϲ���
    for (int mid = 1; mid < limit; mid <<= 1) {
        //���ϲ����䳤�ȵ�һ�룬�ʼ����������Ϊ1�����кϲ�,mid = 1;
        Complex Wn(cos(PI / mid), type * sin(PI / mid));//��λ����
        for (int len = mid << 1, j = 0; j < limit; j += len) {
            //len������ĳ��ȣ�j�ǵ�ǰ��λ�ã�Ҳ���Ǻϲ�������һλ��
            Complex w(1, 0);//�ݣ�һֱ�ˣ��õ�ƽ�������η���
            for (int k = 0; k < mid; ++k, w = w * Wn) {
                //ö����벿�֡������任�õ��Ұ벿�ֵĴ𰸡�wΪwn * k
                Complex x = A[j + k], y = w * A[j + mid + k];//��벿�ֺ��Ұ벿�֡�
                A[j + k] = x + y;//��߼ӡ�
                A[j + mid + k] = x - y;//�ұ߼���
            }
        }
    }
    if (type == 1)return;
    for (int i = 0; i <= limit; ++i) {
        a[i].x /= limit;
        //�����Ҫ����limitҲ���ǲ�����2�������ݡ�����ֵת��Ϊϵ����
    }
}
void solve() {
    while (limit <= n + m) {
        limit <<= 1, l++;
    }
    //��ʼ��r���顣
    for (int i = 0; i < limit; ++i) {
        r[i] = (r[i >> 1] >> 1) | ((i & 1) << (l - 1));
    }
    fft(a, 1);//��a��ϵ��ת��Ϊ��ֵ��ʾ��
    fft(b, 1);//��b��ϵ��ת��Ϊ��ֵ��ʾ��
    for (int i = 0; i <= limit; ++i) {
        //��Ӧ����ˣ��õ���ֵ��ʾ�Ľ⡣
        a[i] = a[i] * b[i];
    }
    fft(a, -1);
    for (int i = 0; i <= n + m; ++i) {
        //ȡ������2������0.5�������롣
        printf("%d ", (int)(a[i].x + 0.5));
    }
    printf("\n");
}
int main() {
    //��ʼ������
    n = 2;
    m = 1;
    a[0] = 1;
    a[1] = 1;
    a[2] = 2;
    b[0] = 2;
    b[1] = 3;
    solve();
    return 0;
}