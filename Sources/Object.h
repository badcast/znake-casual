#pragma once
#include "framework.h"

namespace RoninEngine {
   namespace Runtime {

      extern Transform* create_empty_transform();
      extern GameObject* create_empty();

      GameObject *CreateGameObject();
      GameObject *CreateGameObject(const string& name);


      ///Уничтожает объект после рендера.
      void Destroy(Object* obj);

      /// Уничтожает объект после прошедшего времени.
      void Destroy(Object* obj, float t);

      ///Уничтожает объект принудительно игнорируя все условия его существования.
      void Destroy_Immediate(Object* obj);

      ///Проверка на существование объекта
      bool instanced(Object* obj);

      ///Клонирует объект
      template <typename ObjectType>
      ObjectType* Instantiate(ObjectType* obj);
      ///Клонирует объект
      GameObject* Instantiate(GameObject* obj, Vec2 position, float angle = 0);
      ///Клонирует объект
      GameObject* Instantiate(GameObject* obj, Vec2 position, Transform* parent,
                              bool worldPositionState = true);

      class Object {
            template <typename ObjectType>
            friend ObjectType* Instantiate(ObjectType* obj);
            friend GameObject* Instantiate(GameObject* obj);
            friend GameObject* Instantiate(GameObject* obj, Vec2 position, float angle);
            friend GameObject* Instantiate(GameObject* obj, Vec2 position,
                                           Transform* parent, bool worldPositionState);

            friend void Destroy(Object* obj);
            friend void Destroy(Object* obj, float t);
            friend void Destroy_Immediate(Object* obj);
            string m_name;
            std::size_t id;

         public:
            const string name() const;
            string& name();

            Object();
            Object(const string& nameobj);
            virtual ~Object() = default;

            void Destroy();

            /// Проверка на существования объекта
            operator bool();
      };

      template <typename _based, typename _derived>
      constexpr bool object_base_of() {
         return std::is_base_of<_based, _derived>();
      }

      template <typename _based, typename _derived>
      constexpr bool object_base_of(_based* obj, _derived* compare) {
         return std::is_base_of<_based, _derived>();
      }
   }  // namespace Runtime
}  // namespace RoninEngine
