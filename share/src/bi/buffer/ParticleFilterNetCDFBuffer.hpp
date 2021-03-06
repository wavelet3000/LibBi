/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_BUFFER_PARTICLEFILTERNETCDFBUFFER_HPP
#define BI_BUFFER_PARTICLEFILTERNETCDFBUFFER_HPP

#include "SimulatorNetCDFBuffer.hpp"

namespace bi {
/**
 * Buffer for storing, reading and writing results of a particle filter.
 *
 * @ingroup io_buffer
 */
class ParticleFilterNetCDFBuffer: public SimulatorNetCDFBuffer {
public:
  using SimulatorNetCDFBuffer::writeState;

  /**
   * Constructor.
   *
   * @param m Model.
   * @param file NetCDF file name.
   * @param mode File open mode.
   */
  ParticleFilterNetCDFBuffer(const Model& m, const std::string& file,
      const FileMode mode = READ_ONLY, const SchemaMode schema = DEFAULT);

  /**
   * Constructor.
   *
   * @param m Model.
   * @param P Number of samples in file.
   * @param T Number of times in file.
   * @param file NetCDF file name.
   * @param mode File open mode.
   */
  ParticleFilterNetCDFBuffer(const Model& m, const size_t P, const size_t T,
      const std::string& file, const FileMode mode = READ_ONLY,
      const SchemaMode schema = DEFAULT);

  /**
   * Read particle log-weights.
   *
   * @tparam V1 Vector type.
   *
   * @param k Time index.
   * @param[out] lws Log-weights.
   */
  template<class V1>
  void readLogWeights(const size_t k, V1 lws);

  /**
   * Write particle log-weights.
   *
   * @tparam V1 Vector type.
   *
   * @param k Time index.
   * @param lws Log-weights.
   */
  template<class V1>
  void writeLogWeights(const size_t k, const V1 lws);

  /**
   * Read particle ancestors.
   *
   * @tparam V1 Vector type.
   *
   * @param k Time index.
   * @param[out] a Ancestry.
   */
  template<class V1>
  void readAncestors(const size_t k, V1 a);

  /**
   * Write particle ancestors.
   *
   * @tparam V1 Vector type.
   *
   * @param k Time index.
   * @param a Ancestry.
   */
  template<class V1>
  void writeAncestors(const size_t k, const V1 a);

  /**
   * Write dynamic state and ancestors.
   *
   * @tparam M1 Matrix type.
   * @tparam V1 Vector type.
   *
   * @param k Time index.
   * @param X State.
   * @param as Ancestors.
   */
  template<class M1, class V1>
  void writeState(const size_t k, const M1 X, const V1 as);

  /**
   * Write marginal log-likelihood estimate.
   *
   * @param ll Marginal log-likelihood estimate.
   */
  void writeLL(const real ll);

protected:
  /**
   * Set up structure of NetCDF file.
   */
  void create();

  /**
   * Map structure of existing NetCDF file.
   */
  void map();

  /**
   * Ancestors variable.
   */
  int aVar;

  /**
   * Log-weights variable.
   */
  int lwVar;

  /**
   * Marginal log-likelihood estimate variable.
   */
  int llVar;
};
}

template<class V1>
void bi::ParticleFilterNetCDFBuffer::readLogWeights(const size_t k, V1 lws) {
  if (schema == FLEXI) {
    size_t start = readStart(k);
    size_t len = readLen(k);
    BI_ERROR(lws.size() == len);
    readRange(lwVar, start, lws);
  } else {
    readVector(lwVar, k, lws);
  }
}

template<class V1>
void bi::ParticleFilterNetCDFBuffer::writeLogWeights(const size_t k,
    const V1 lws) {
  if (schema == FLEXI) {
    size_t start = readStart(k);
    size_t len = readLen(k);
    BI_ERROR(lws.size() == len);
    writeRange(lwVar, start, lws);
  } else {
    writeVector(lwVar, k, lws);
  }
}

template<class V1>
void bi::ParticleFilterNetCDFBuffer::readAncestors(const size_t k, V1 as) {
  if (schema == FLEXI) {
    size_t start = readStart(k);
    size_t len = readLen(k);
    BI_ERROR(as.size() == len);
    readRange(aVar, start, as);
  } else {
    readVector(aVar, k, as);
  }
}

template<class V1>
void bi::ParticleFilterNetCDFBuffer::writeAncestors(const size_t k,
    const V1 as) {
  if (schema == FLEXI) {
    size_t start = readStart(k);
    size_t len = readLen(k);
    BI_ERROR(as.size() == len);
    writeRange(aVar, start, as);
  } else {
    writeVector(aVar, k, as);
  }
}

template<class M1, class V1>
void bi::ParticleFilterNetCDFBuffer::writeState(const size_t k, const M1 X,
    const V1 as) {
  SimulatorNetCDFBuffer::writeState(k, X);
  writeAncestors(k, as);
}

#endif
