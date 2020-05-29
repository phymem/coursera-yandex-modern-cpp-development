#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <chrono>
#include <vector>

struct ProfileData {
	const char* m_name;
	double m_min_time;
	double m_max_time;
	double m_tot_time;
	unsigned int m_num;

	ProfileData(const char* f_name = nullptr) :
		m_name(f_name),
		m_min_time(0),
		m_max_time(0),
		m_tot_time(0),
		m_num(0) {}

	static std::vector<ProfileData> m_data;

	static ProfileData* findByName(const char* f_name) {
		for (unsigned int i = 0; i < m_data.size(); ++i) {
			ProfileData* d = &m_data[i];
			if (!strcmp(d->m_name, f_name))
				return d;
		}
		m_data.push_back(ProfileData(f_name));
		return &m_data[m_data.size() - 1];
	}

	static void dump() {
		fprintf(stderr, "\n\n");
		for (const ProfileData& d : m_data) {
			fprintf(stderr, "%16s: max=%7.3f min=%7.3f avg=%7.3f num=%3d tot=%7.3f\n",
				d.m_name,
				d.m_max_time,
				d.m_min_time,
				d.m_tot_time / d.m_num,
				d.m_num,
				d.m_tot_time);
		}
	}
};

struct ProfileSensor {
	const char* m_name;
	std::chrono::high_resolution_clock::time_point m_start;

	ProfileSensor(const char* f_name) :
		m_name(f_name), m_start(std::chrono::high_resolution_clock::now()) {}

	~ProfileSensor() {

		double dur = std::chrono::duration<double, std::milli>(
			std::chrono::high_resolution_clock::now() - m_start).count();

		ProfileData* d = ProfileData::findByName(m_name);

		if (!d->m_min_time || dur < d->m_min_time)
			d->m_min_time = dur;
		if (!d->m_max_time || dur > d->m_max_time)
			d->m_max_time = dur;

		d->m_tot_time += dur;
		d->m_num++;
	}
};

#endif // __PROFILE_H__
