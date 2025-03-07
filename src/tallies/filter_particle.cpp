#include "openmc/tallies/filter_particle.h"

#include "openmc/xml_interface.h"

namespace openmc {

void
ParticleFilter::from_xml(pugi::xml_node node)
{
  auto particles = get_node_array<int>(node, "bins");

  // Convert to vector of Particle::Type
  std::vector<Particle::Type> types;
  for (auto& p : particles) {
    types.push_back(static_cast<Particle::Type>(p - 1));
  }
  this->set_particles(types);
}

void
ParticleFilter::set_particles(gsl::span<Particle::Type> particles)
{
  // Clear existing particles
  particles_.clear();
  particles_.reserve(particles.size());

  // Set particles and number of bins
  for (auto p : particles) {
    particles_.push_back(p);
  }
  n_bins_ = particles_.size();
}

void
ParticleFilter::get_all_bins(const Particle* p, int estimator,
                             FilterMatch& match) const
{
  for (auto i = 0; i < particles_.size(); i++) {
    if (particles_[i] == p->type_) {
      match.bins_.push_back(i);
      match.weights_.push_back(1.0);
    }
  }
}

void
ParticleFilter::to_statepoint(hid_t filter_group) const
{
  Filter::to_statepoint(filter_group);
  std::vector<int> particles;
  for (auto p : particles_) {
    particles.push_back(static_cast<int>(p) + 1);
  }
  write_dataset(filter_group, "bins", particles);
}

std::string
ParticleFilter::text_label(int bin) const
{
  switch (particles_[bin]) {
  case Particle::Type::neutron:
    return "Particle: neutron";
  case Particle::Type::photon:
    return "Particle: photon";
  case Particle::Type::electron:
    return "Particle: electron";
  case Particle::Type::positron:
    return "Particle: positron";
  }
  UNREACHABLE();
}

} // namespace openmc
