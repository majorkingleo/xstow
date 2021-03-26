/**
 * @file
 * @todo describe file content
 * @author Copyright (c) 2011 Salomon Automation GmbH
 */

#ifndef _Tools_MAP_UTILS_H
#define _Tools_MAP_UTILS_H

#include <map>
#include <set>
#include <vector>
#include <algorithm>

namespace Tools {

template <class Key, class Val> std::set<Key>
		getKeySet (const std::map<Key, Val> &inMap) {

	typename std::map<Key, Val>::const_iterator it;
	std::set<Key> outSet;
	for (it = inMap.begin(); it != inMap.end(); it++) {
		outSet.insert(it->first);
	}

	return outSet;
}


template <class Key, class Val> std::vector<Key>
		getKeySetAsVector (const std::map<Key, Val> &inMap) {

	std::set<Key> outSet = getKeySet(inMap);
	typename std::set<Key>::const_iterator it;
	std::vector<Key> outVec;
	for (it = outSet.begin(); it != outSet.end(); it++) {
		outVec.push_back(*it);
	}

	return outVec;

}

template <class Key, class Val, class Sorter> std::vector<Key>
		getKeySetAsVector (const std::map<Key, Val> &inMap,
		const Sorter &sorter) {

		std::vector<Key> outVec =  getKeySetAsVector(inMap);
		std::sort(outVec.begin(), outVec.end(), sorter);
		return outVec;

}



} // namespace Tools

#endif  // _Tools_MAP_UTILS_H
