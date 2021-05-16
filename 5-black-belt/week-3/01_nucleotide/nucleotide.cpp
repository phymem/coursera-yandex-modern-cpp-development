#include <cstdint>
#include <cassert>


struct Nucleotide {
	char Symbol;
	size_t Position;
	int ChromosomeNum;
	int GeneNum;
	bool IsMarked;
	char ServiceInfo;
};


struct CompactNucleotide {
	uint32_t Position : 32;
	uint8_t ServiceInfo : 8;
	uint8_t ChromosomeNum : 6;
	uint8_t Symbol : 2;
	uint16_t GeneNum : 15;
	uint16_t IsMarked : 1;
};
static_assert(sizeof(CompactNucleotide) <= 8);
static_assert(alignof(CompactNucleotide) == 4); // dont' use '#pragma pack'


bool operator == (const Nucleotide& lhs, const Nucleotide& rhs) {
	return (lhs.Symbol == rhs.Symbol)
		&& (lhs.Position == rhs.Position)
		&& (lhs.ChromosomeNum == rhs.ChromosomeNum)
		&& (lhs.GeneNum == rhs.GeneNum)
		&& (lhs.IsMarked == rhs.IsMarked)
		&& (lhs.ServiceInfo == rhs.ServiceInfo);
}


CompactNucleotide Compress(const Nucleotide& n) {
	CompactNucleotide retval;

	switch (n.Symbol) {
	case 'A':
		retval.Symbol = 0;
		break;
	case 'C':
		retval.Symbol = 1;
		break;
	case 'G':
		retval.Symbol = 2;
		break;
	case 'T':
		retval.Symbol = 3;
		break;
	default:
		assert(!"unknown Necleotide::Symbol value");
	}

	retval.Position = n.Position;
	retval.ChromosomeNum = n.ChromosomeNum;
	retval.GeneNum = n.GeneNum;
	retval.IsMarked = n.IsMarked;
	retval.ServiceInfo = n.ServiceInfo;

	return retval;
};


Nucleotide Decompress(const CompactNucleotide& cn) {
	Nucleotide retval;

	switch (cn.Symbol) {
	case 0:
		retval.Symbol = 'A';
		break;
	case 1:
		retval.Symbol = 'C';
		break;
	case 2:
		retval.Symbol = 'G';
		break;
	case 3:
		retval.Symbol = 'T';
		break;
	default:
		assert(!"unknown CompactNecleotide::Symbol value");
	}

	retval.Position = cn.Position;
	retval.ChromosomeNum = cn.ChromosomeNum;
	retval.GeneNum = cn.GeneNum;
	retval.IsMarked = cn.IsMarked;
	retval.ServiceInfo = cn.ServiceInfo;

	return retval;
}
