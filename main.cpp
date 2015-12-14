#include <bits/stdc++.h>

using namespace std;

class CSuffixArray {
private:
    enum {
        S_TYPE, L_TYPE, LMS_TYPE
    };
    int alphabetSize;
    vector<bool> sufType;
    vector<int> s;
    vector<int> lmsS;
    vector<int> lmsSuffixArray;
    vector<int> lmsPos;
    vector<int> bucketSize;
    vector<int> bucketHeads;
    vector<int> bucketTails;
    vector<int> suffixArray;
    vector<int> lcp;

    inline bool isSType(int pos) {
        return sufType[pos] == S_TYPE;
    }

    inline bool isLType(int pos) {
        return sufType[pos] == L_TYPE;
    }

    inline bool isLMSType(int pos) {
        return (pos == 0 ? false : sufType[pos] == S_TYPE && sufType[pos - 1] == L_TYPE);
    }

    bool cmpLMSSubstr(int pos1, int pos2) {
        int i = 0;
        while (1) {
            if (i > 0 && isLMSType(i + pos1) && isLMSType(i + pos2)) {
                return true;
            }
            if (isLMSType(i + pos1) != isLMSType(i + pos2) || s[i + pos1] != s[i + pos2]) {
                return false;
            }
            ++i;
        }
    }

    void classifyCharacters() {
        sufType.assign(s.size(), S_TYPE);
        for (int i = s.size() - 2; i >= 0; i--) {
            if (s[i] > s[i + 1]) {
                sufType[i] = L_TYPE;
            } else if (s[i] < s[i + 1]) {
                sufType[i] = S_TYPE;
            } else {
                sufType[i] = sufType[i + 1];
            }
        }
    }

    void initBucketHeads() {
        int j = 1;
        bucketHeads.clear();
        for (int i = 0; i < bucketSize.size(); ++i) {
            bucketHeads.push_back(j);
            j += bucketSize[i];
        }
    }

    void initBucketTails() {
        int j = 1;
        bucketTails.clear();
        for (int i = 0; i < bucketSize.size(); ++i) {
            j += bucketSize[i];
            bucketTails.push_back(j - 1);
        }
    }

    void induceSortLMS() {
        initBucketTails();
        for (int i = 0; i < s.size() - 1; i++) {
            if (!isLMSType(i))
                continue;
            suffixArray[bucketTails[s[i]]] = i;
            bucketTails[s[i]]--;
        }
        suffixArray[0] = s.size() - 1;
    }

    void induceSortL() {
        initBucketHeads();
        for (int i = 0; i < suffixArray.size(); i++) {
            if (suffixArray[i] <= 0) {
                 continue;
            }
            int j = suffixArray[i] - 1;
            if (j < 0 || sufType[j] != L_TYPE) {
                continue;
            }
            suffixArray[bucketHeads[s[j]]] = j;
            bucketHeads[s[j]]++;
        }
    }

    void induceSortS() {
        initBucketTails();
        for (int i = suffixArray.size() - 1; i >= 0; i--) {
            int j = suffixArray[i] - 1;
            if (j < 0 || sufType[j] != S_TYPE) {
                continue;
            }
            suffixArray[bucketTails[s[j]]] = j;
            bucketTails[s[j]]--;
        }
    }

    int makeNewString() {
        vector<int> lmsNames(s.size(), -1);
        int maxName = 0;
        lmsNames[suffixArray[0]] = maxName;
        int lastPos = suffixArray[0];
        for (int i = 1; i < suffixArray.size(); i++) {
            int currentPos = suffixArray[i];
            if (!isLMSType(currentPos)) {
                continue;
            }
            if (!cmpLMSSubstr(lastPos, currentPos)) {
                 maxName++;
            }
            lastPos = currentPos;
            lmsNames[currentPos] = maxName;
        }
        for (int i = 0; i < lmsNames.size(); i++) {
            if (lmsNames[i] == -1) {
                continue;
            }
            lmsPos.push_back(i);
            lmsS.push_back(lmsNames[i]);
        }
        return maxName + 1;
    }

    void placeLMSSuf() {
        suffixArray.assign(s.size(), -1);
        initBucketTails();
        for (int i = lmsSuffixArray.size() - 1; i > 1; i--) {
            int currentPos = lmsPos[lmsSuffixArray[i]];
            suffixArray[bucketTails[s[currentPos]]] = currentPos;
            bucketTails[s[currentPos]]--;
        }
        suffixArray[0] = s.size() - 1;
    }

    void sais() {
        s.push_back(0);

        classifyCharacters();

        bucketSize.assign(alphabetSize, 0);
        for (int i = 0; i < s.size() - 1; i++) {
            bucketSize[s[i]]++;
        }

        suffixArray.assign(s.size(), -1);
        induceSortLMS();
        induceSortL();
        induceSortS();

        int lmsAlphabetSize = makeNewString();

        if (lmsAlphabetSize == lmsS.size()) {

            lmsSuffixArray.assign(lmsS.size() + 1, -1);
            lmsSuffixArray[0] = lmsS.size();
            for (int i = 0; i < lmsS.size(); i++) {
                lmsSuffixArray[lmsS[i] + 1] = i;
            }

        } else {
            lmsSuffixArray = CSuffixArray(lmsS, lmsAlphabetSize).getSuffixArray();
        }

        placeLMSSuf();
        induceSortL();
        induceSortS();
    }

    void genLcp() {
        vector<int> invSuffixArray(s.size());
        lcp.assign(s.size(), 0);
        for (int i = 0; i < s.size(); i++) {
            invSuffixArray[suffixArray[i]] = i;
        }
        int l = 0;
        for (int i = 0; i < s.size(); i++) {
            l = max(0, l - 1);
            int p = invSuffixArray[i];
            if (p + 1 < s.size())
                while(s[i + l] == s[suffixArray[p + 1] + l])
                    ++l;
            lcp[p] = l;
        }
    }
public:
    CSuffixArray(vector<int> str, int alph = 256) {
        s = str;
        alphabetSize = alph;
        sais();
    }
    CSuffixArray(string str, int alph = 256) {
        for (int i = 0; i < str.length(); i++) {
            s.push_back(str[i]);
        }
        alphabetSize = alph;
        sais();
    }

    vector<int> getSuffixArray() {
        return suffixArray;
    }

    vector<int> getLCP() {
        if (lcp.empty()) {
            genLcp();
        }
        return lcp;
    }

    int getDifferentSubstringsCount() {
        if (lcp.empty()) {
            genLcp();
        }
        int ans = s.size() - suffixArray[0];
        for (int i = 1; i < suffixArray.size(); i++) {
            ans += (suffixArray.size() - suffixArray[i]) - lcp[i - 1];
        }
        return ans - s.size();
    }
};

int main()
{
    string s;
    int k;
    cin >> k >> s;
    s += s;
    for (int i = 0; i * 2 < s.size(); i++) {
        cout << CSuffixArray(s.substr(i, k)).getDifferentSubstringsCount() << ' ';
    }
    return 0;
}
