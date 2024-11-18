#pragma once
#include "Common.h"

namespace Net
{
    class Session;
    class Message;

    namespace Routing
    {

       #define ROUTER_BASE_COMPONENT 

      class INTERFACE_COMPONENT Transact {
      public:
        virtual ~Transact() = default;
        virtual std::string current_path() const = 0;
        virtual void next_path() = 0;
      };



      class ROUTER_BASE_COMPONENT RoutTransact : public Transact
      {
      public:
        RoutTransact();
        RoutTransact(const std::string& path);

        virtual void next_path() noexcept(false) override;
        virtual std::string current_path() const noexcept(false) override;

      private:
        std::string m_path;
      };



      class  INTERFACE_COMPONENT RoutHandle {
      public:
        virtual ~RoutHandle() = default;
        virtual void route(const std::shared_ptr<Session>&, const std::shared_ptr<Request>&) noexcept(false) = 0;
        void forward_transact(const std::shared_ptr<Transact>&);

      protected:
        std::shared_ptr<Transact> m_transact;
      };



      class SERVER_COMPONENT ROUTER_BASE_COMPONENT Router : public RoutHandle {
      public:
        Router() = default;
        Router(const Router& router);
        Router(Router&& router) noexcept(true);

        virtual ~Router() = default;

      private:
        using Function = std::function<void(const std::shared_ptr<Session>&,
          const std::shared_ptr<Request>&,
          const std::shared_ptr<RoutHandle>&)>;



        class ROUTER_BASE_COMPONENT FuncHandle : public RoutHandle {
        public:
          FuncHandle() = default;
          FuncHandle(const Function& callback, const std::shared_ptr<RoutHandle>& handle);

        private:
          virtual void route(const std::shared_ptr<Session>&, const std::shared_ptr<Request>& ) noexcept(false) override;

        private:
          Function m_callback;
          std::shared_ptr<RoutHandle> m_next_handle;
        };



        class ROUTER_BASE_COMPONENT EndHandle : public RoutHandle {
        public:
          EndHandle() = default;
        private:
          virtual void route(const std::shared_ptr<Session>&, const std::shared_ptr<Request>& ) noexcept(false) override;
        };



      public:
        virtual void route(const std::shared_ptr<Session>& req, 
          const std::shared_ptr<Request>& res,
          const std::shared_ptr<Transact>& transact) noexcept(false);

        template<typename... Fs>
        void add(const std::string& key, Fs&&... callbacks) noexcept(true);
        void remove(const std::string& key) noexcept(true);

      private:
        virtual void route(const std::shared_ptr<Session>&, const std::shared_ptr<Request>&) override;

      private:
        template<typename F, typename... Fs>
        std::shared_ptr<RoutHandle> create_handle(const F& callback, Fs&&... callbacks);
        template<typename F>
        std::shared_ptr<RoutHandle> create_handle(const F& callback);

        std::shared_ptr<RoutHandle> create_handle(const std::shared_ptr<Router>& router);

      private:
        mutable std::mutex m_mutex;
        std::unordered_map<std::string, std::shared_ptr<RoutHandle>> m_router_map;
      };



      template <typename ... Fs>
      void Router::add(const std::string& key, Fs&&... callbacks) noexcept(true) 
      {
        std::shared_ptr<RoutHandle> head_handle = create_handle(std::forward<Fs>(callbacks)...);

        std::unique_lock<std::mutex> ul(m_mutex);
        m_router_map.insert({ key, head_handle });
        ul.unlock();
      }



      template <typename F, typename ... Fs>
      std::shared_ptr<RoutHandle> Router::create_handle(const F& callback, Fs&&... callbacks) {
        return std::make_shared<FuncHandle>(callback, create_handle(std::forward<Fs>(callbacks)...));
      }

      template <typename F>
      std::shared_ptr<RoutHandle> Router::create_handle(const F& callback) {
        return std::make_shared<FuncHandle>(callback, std::make_shared<EndHandle>());
      }


    }
}
