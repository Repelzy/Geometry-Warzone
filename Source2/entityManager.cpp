#include "entityManager.h"

EntityManager::EntityManager() {
}


void EntityManager::removeDeadEntities(EntityVec& vec)
{
    vec.erase(std::remove_if(vec.begin(), vec.end(), [](auto& entity) {
        return !entity->isActive();
        }), vec.end());
}

void EntityManager::update() {
    // Add entities from m_toAdd to m_entities and m_entityMap
    for (auto& e : m_toAdd) {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }
    // Remove dead entities from m_entities
    removeDeadEntities(m_entities);

    // Remove dead entities from each tag group in m_entityMap
    for (auto& [tag, EntityVec] : m_entityMap) {
        removeDeadEntities(EntityVec);
    }

    // Clear the list of entities to add
    m_toAdd.clear();
}


std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
    auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_toAdd.push_back(e);

    return e;
}

const EntityVec& EntityManager::getEntities() {
    return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag) {
    return m_entityMap[tag];
}
